#include "prx/libSceAgcDriver/Graphics/include/ShaderResources.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <cstring>
#include <set>
#include <string>

namespace AgcDriver::Graphics {
namespace {

bool overlap(std::uint64_t first, std::size_t firstSize, std::uint64_t second, std::size_t secondSize) {
    return first < second + secondSize && second < first + firstSize;
}

struct Binding {
    VkDescriptorSetLayoutBinding layout;
    std::vector<std::size_t> allocations;
};

const char* roleName(ShaderRecompiler::DescriptorRole role) {
    switch (role) {
        case ShaderRecompiler::DescriptorRole::GuestBuffers: return "GuestBuffers";
        case ShaderRecompiler::DescriptorRole::GuestImages: return "GuestImages";
        case ShaderRecompiler::DescriptorRole::GuestSamplers: return "GuestSamplers";
        case ShaderRecompiler::DescriptorRole::Gds: return "Gds";
        case ShaderRecompiler::DescriptorRole::BdaPagetable: return "BdaPagetable";
        case ShaderRecompiler::DescriptorRole::FaultBuffer: return "FaultBuffer";
        case ShaderRecompiler::DescriptorRole::FlattenedSrt: return "FlattenedSrt";
        case ShaderRecompiler::DescriptorRole::ShaderData: return "ShaderData";
    }
    throw std::runtime_error("AGC graphics: unknown descriptor role");
}

const char* kindName(ShaderRecompiler::DescriptorKind kind) {
    switch (kind) {
        case ShaderRecompiler::DescriptorKind::UniformBuffer: return "UniformBuffer";
        case ShaderRecompiler::DescriptorKind::StorageBuffer: return "StorageBuffer";
        case ShaderRecompiler::DescriptorKind::UniformTexelBuffer: return "UniformTexelBuffer";
        case ShaderRecompiler::DescriptorKind::StorageTexelBuffer: return "StorageTexelBuffer";
        case ShaderRecompiler::DescriptorKind::SampledImage: return "SampledImage";
        case ShaderRecompiler::DescriptorKind::StorageImage: return "StorageImage";
        case ShaderRecompiler::DescriptorKind::Sampler: return "Sampler";
    }
    throw std::runtime_error("AGC graphics: unknown descriptor kind");
}

}

ShaderResources::ShaderResources(const Context& context, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes) : ShaderResources(context, std::array<CompiledShader, 2>{{{ShaderRecompiler::ShaderStage::Vertex, &vertex, 0}, {ShaderRecompiler::ShaderStage::Fragment, &fragment, static_cast<std::uint32_t>(vertex.pushConstants.size())}}}, target, indexAddress, indexBytes) {}

ShaderResources::ShaderResources(const Context& context, std::span<const CompiledShader> shaders, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes, std::span<const GuestMemorySnapshot> snapshots) : context(context), guestMemory(context) {
    prepareAddressBindings(shaders, snapshots);
    build(shaders, &target, indexAddress, indexBytes);
}

ShaderResources::ShaderResources(const Context& context, const CompiledShader& compute, std::span<const GuestMemorySnapshot> snapshots) : context(context), guestMemory(context) {
    Require(compute.stage == ShaderRecompiler::ShaderStage::Compute, "compute resources require a compute shader");
    prepareAddressBindings(std::span<const CompiledShader>(&compute, 1), snapshots);
    build(std::span<const CompiledShader>(&compute, 1), nullptr, 0, 0);
}

void ShaderResources::build(std::span<const CompiledShader> shaders, const ColorTarget* target, std::uint64_t indexAddress, std::size_t indexBytes) {
    try {
        Require(!shaders.empty() && context.limits.maxBoundDescriptorSets >= 1, "shader descriptor set exceeds device limits");
        std::vector<Binding> bindings;
        std::set<std::uint32_t> occupied;
        std::uint64_t storageBuffers = 0;
        for (const auto& shader : shaders) {
            Require(shader.program != nullptr, "missing compiled shader");
            const VkShaderStageFlags flags = VulkanStage(shader.stage);
            std::uint64_t stageDescriptors = 0;
            for (const auto& binding : shader.program->bindings) {
                Require(binding.descriptorSet == 0, "unexpected descriptor set: every shader resource must use descriptor set zero");
                Require(occupied.insert(binding.binding).second, "duplicate shader binding");
                const bool addressRole = binding.role == ShaderRecompiler::DescriptorRole::BdaPagetable || binding.role == ShaderRecompiler::DescriptorRole::FaultBuffer;
                const bool bufferRole = addressRole || binding.role == ShaderRecompiler::DescriptorRole::GuestBuffers || binding.role == ShaderRecompiler::DescriptorRole::ShaderData || binding.role == ShaderRecompiler::DescriptorRole::FlattenedSrt;
                const bool imageRole = binding.role == ShaderRecompiler::DescriptorRole::GuestImages || binding.role == ShaderRecompiler::DescriptorRole::GuestSamplers;
                Require(!imageRole, "sampled and storage image resources are not implemented");
                Require(bufferRole, std::string("unsupported descriptor role ") + roleName(binding.role));
                Require(binding.kind == ShaderRecompiler::DescriptorKind::StorageBuffer, std::string("unsupported descriptor kind ") + kindName(binding.kind) + " for role " + roleName(binding.role) + ": only StorageBuffer is supported");
                Require(!binding.readOnly, "read-only descriptors are unsupported because the recompiler emits no NonWritable decoration");
                Require(binding.count != 0, "empty descriptor binding");
                stageDescriptors += binding.count;
                storageBuffers += binding.count;
                Require(stageDescriptors <= context.limits.maxPerStageDescriptorStorageBuffers && stageDescriptors <= context.limits.maxPerStageResources, "shader descriptors exceed per-stage limits");
                Binding item{{binding.binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, binding.count, flags, nullptr}, {}};
                if (binding.role == ShaderRecompiler::DescriptorRole::GuestBuffers) {
                    Require(binding.guestDescriptor.size() == static_cast<std::uint64_t>(binding.count) * 4, "guest buffer descriptor must contain four DWORDs per array element");
                    for (std::uint32_t element = 0; element < binding.count; ++element) item.allocations.push_back(addGuestBuffer(std::span<const std::uint32_t>(binding.guestDescriptor).subspan(static_cast<std::size_t>(element) * 4, 4), target, indexAddress, indexBytes));
                } else if (addressRole) {
                    item.allocations.push_back(allocations.size());
                    allocations.push_back({0, 0, false, nullptr, binding.role});
                } else {
                    Require(binding.count == 1, "shader data and flattened SRT descriptors must not be arrays");
                    Require(!binding.guestDescriptor.empty(), "empty shader data descriptor");
                    item.allocations.push_back(addDataBuffer(binding.guestDescriptor));
                }
                bindings.push_back(std::move(item));
            }
        }
        Require(storageBuffers <= context.limits.maxDescriptorSetStorageBuffers, "pipeline descriptors exceed device limits");
        guestMemory.Upload(usesBda);
        if (usesBda) bda = std::make_unique<BdaResources>(context, guestMemory);
        std::vector<VkDescriptorSetLayoutBinding> description;
        for (const auto& binding : bindings) description.push_back(binding.layout);
        VkDescriptorSetLayoutCreateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        info.bindingCount = static_cast<std::uint32_t>(description.size());
        info.pBindings = description.data();
        Check(context.Function<PFN_vkCreateDescriptorSetLayout>("vkCreateDescriptorSetLayout")(context.device, &info, nullptr, &_layout), "vkCreateDescriptorSetLayout");
        if (bindings.empty()) return;
        const VkDescriptorPoolSize size{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, static_cast<std::uint32_t>(storageBuffers)};
        VkDescriptorPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        poolInfo.maxSets = 1;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &size;
        Check(context.Function<PFN_vkCreateDescriptorPool>("vkCreateDescriptorPool")(context.device, &poolInfo, nullptr, &pool), "vkCreateDescriptorPool");
        VkDescriptorSetAllocateInfo allocation{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocation.descriptorPool = pool;
        allocation.descriptorSetCount = 1;
        allocation.pSetLayouts = &_layout;
        Check(context.Function<PFN_vkAllocateDescriptorSets>("vkAllocateDescriptorSets")(context.device, &allocation, &_set), "vkAllocateDescriptorSets");
        for (const auto& binding : bindings) {
            std::vector<VkDescriptorBufferInfo> buffers;
            for (const auto index : binding.allocations) buffers.push_back(descriptor(allocations[index]));
            VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
            write.dstSet = _set;
            write.dstBinding = binding.layout.binding;
            write.descriptorCount = binding.layout.descriptorCount;
            write.descriptorType = binding.layout.descriptorType;
            write.pBufferInfo = buffers.data();
            context.Function<PFN_vkUpdateDescriptorSets>("vkUpdateDescriptorSets")(context.device, 1, &write, 0, nullptr);
        }
    } catch (...) {
        release();
        throw;
    }
}

std::size_t ShaderResources::addGuestBuffer(std::span<const std::uint32_t> words, const ColorTarget* target, std::uint64_t indexAddress, std::size_t indexBytes) {
    Require(words.size() == 4, "buffer descriptor must contain four DWORDs");
    Require((words[1] & 0xffff0000u) == 0, "strided or swizzled buffer descriptors are unsupported");
    Require((words[3] & ~0x0007ffffu) == 0x31000000u, "only raw buffer bounds with resource level one and no index stride or add-TID addressing are supported");
    const auto address = static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1] & 0xffffu) << 32u);
    const auto size = static_cast<std::size_t>(words[2]);
    Require(address != 0, "null shader buffer descriptor address");
    Require(size != 0, "empty shader buffer descriptor");
    Require(size <= context.limits.maxStorageBufferRange, "shader buffer exceeds descriptor range limit");
    GuestMemory::CheckRange(reinterpret_cast<const void*>(address), size, 1, true);
    Require(target == nullptr || !overlap(address, size, target->address, target->bytes), "shader buffer aliases the render target");
    Require(!overlap(address, size, indexAddress, indexBytes), "writable shader buffer aliases the index buffer");
    guestMemory.AddWritable(address, size);
    allocations.push_back({address, size, true, nullptr});
    return allocations.size() - 1;
}

std::size_t ShaderResources::addDataBuffer(std::span<const std::uint32_t> words) {
    const auto size = words.size() * sizeof(std::uint32_t);
    Require(size <= context.limits.maxStorageBufferRange, "shader data buffer exceeds descriptor range limit");
    auto buffer = std::make_unique<Buffer>(context, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    std::memcpy(buffer->Bytes().data(), words.data(), size);
    allocations.push_back({0, size, false, std::move(buffer)});
    return allocations.size() - 1;
}

ShaderResources::~ShaderResources() {
    release();
}

void ShaderResources::release() noexcept {
    if (pool) context.Function<PFN_vkDestroyDescriptorPool>("vkDestroyDescriptorPool")(context.device, pool, nullptr);
    if (_layout) context.Function<PFN_vkDestroyDescriptorSetLayout>("vkDestroyDescriptorSetLayout")(context.device, _layout, nullptr);
}

VkDescriptorSetLayout ShaderResources::Layout() const {
    return _layout;
}

void ShaderResources::Bind(VkCommandBuffer commands, VkPipelineBindPoint bindPoint, VkPipelineLayout layout) const {
    if (_set == VK_NULL_HANDLE) return;
    context.Function<PFN_vkCmdBindDescriptorSets>("vkCmdBindDescriptorSets")(commands, bindPoint, layout, 0, 1, &_set, 0, nullptr);
}

void ShaderResources::WriteBack() {
    if (bda) bda->CheckFault();
    guestMemory.WriteBack();
}

}
