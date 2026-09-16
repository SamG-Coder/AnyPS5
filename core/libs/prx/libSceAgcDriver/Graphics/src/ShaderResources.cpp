#include "prx/libSceAgcDriver/Graphics/include/ShaderResources.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <algorithm>
#include <limits>
#include <map>
#include <set>

namespace AgcDriver::Graphics {
namespace {

bool overlap(std::uint64_t first, std::size_t firstSize, std::uint64_t second, std::size_t secondSize) {
    return first < second + secondSize && second < first + firstSize;
}

struct Binding {
    VkDescriptorSetLayoutBinding layout;
    std::size_t allocation;
};

}

ShaderResources::ShaderResources(const Context& context, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes) : ShaderResources(context, std::array<CompiledShader, 2>{{{ShaderRecompiler::ShaderStage::Vertex, &vertex, 0}, {ShaderRecompiler::ShaderStage::Fragment, &fragment, StagePushConstantBytes}}}, target, indexAddress, indexBytes) {}

ShaderResources::ShaderResources(const Context& context, std::span<const CompiledShader> shaders, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes) : context(context), _layouts(shaders.size()), _sets(shaders.size()) {
    Require(!shaders.empty() && context.limits.maxBoundDescriptorSets >= shaders.size(), "graphics descriptor sets exceed device limits");
    std::vector<std::vector<Binding>> bindings(shaders.size());
    std::map<VkDescriptorType, std::uint32_t> poolCounts;
    try {
        for (std::uint32_t stage = 0; stage < shaders.size(); ++stage) {
            Require(shaders[stage].program != nullptr, "missing compiled shader");
            std::set<std::uint32_t> occupied;
            std::uint32_t uniforms = 0;
            std::uint32_t storage = 0;
            for (const auto& binding : shaders[stage].program->bindings) {
                Require(binding.descriptorSet == stage && binding.count == 1, "unexpected descriptor set or descriptor array");
                Require(occupied.insert(binding.binding).second, "duplicate shader binding");
                const bool storageBuffer = binding.kind == ShaderRecompiler::DescriptorKind::StorageBuffer;
                const bool writable = storageBuffer && !binding.readOnly;
                Require(storageBuffer || binding.kind == ShaderRecompiler::DescriptorKind::UniformBuffer, "image, sampler and texel-buffer descriptors are unsupported");
                Require(binding.guestDescriptor.size() == 4, "buffer descriptor must contain four DWORDs");
                const auto& words = binding.guestDescriptor;
                Require((words[1] & 0xffff0000u) == 0, "strided or swizzled buffer descriptors are unsupported");
                Require((words[3] & ~0x0007ffffu) == 0x31000000u, "only raw buffer bounds with resource level one and no index stride or add-TID addressing are supported");
                const auto address = static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1] & 0xffffu) << 32u);
                const auto size = static_cast<std::size_t>(words[2]);
                Require(size != 0, "empty shader buffer descriptor");
                const auto maxRange = storageBuffer ? context.limits.maxStorageBufferRange : context.limits.maxUniformBufferRange;
                Require(size <= maxRange, "shader buffer exceeds descriptor range limit");
                GuestMemory::CheckRange(reinterpret_cast<const void*>(address), size, 1, writable);
                Require(!overlap(address, size, target.address, target.bytes), "shader buffer aliases the render target");
                Require(!writable || !overlap(address, size, indexAddress, indexBytes), "writable shader buffer aliases the index buffer");
                auto allocation = allocations.size();
                for (std::size_t i = 0; i < allocations.size(); ++i) {
                    const auto& existing = allocations[i];
                    if (!overlap(address, size, existing.address, existing.size)) continue;
                    Require(address == existing.address && size == existing.size, "partially overlapping shader buffers are unsupported");
                    Require(!writable && !existing.writable, "aliased writable shader descriptors are unsupported");
                    allocation = i;
                    break;
                }
                if (allocation == allocations.size()) {
                    auto buffer = std::make_unique<Buffer>(context, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
                    GuestMemory::Read(address, buffer->Bytes());
                    allocations.push_back({address, size, writable, std::move(buffer)});
                } else {
                    allocations[allocation].writable = allocations[allocation].writable || writable;
                }
                const auto type = storageBuffer ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                if (storageBuffer) ++storage;
                else ++uniforms;
                ++poolCounts[type];
                const VkShaderStageFlags flags = VulkanStage(shaders[stage].stage);
                bindings[stage].push_back({{binding.binding, type, 1, flags, nullptr}, allocation});
            }
            Require(uniforms <= context.limits.maxPerStageDescriptorUniformBuffers && storage <= context.limits.maxPerStageDescriptorStorageBuffers, "shader descriptors exceed per-stage limits");
            Require(shaders[stage].program->bindings.size() <= context.limits.maxPerStageResources, "shader resources exceed per-stage limit");
            std::vector<VkDescriptorSetLayoutBinding> description;
            for (const auto& binding : bindings[stage]) description.push_back(binding.layout);
            VkDescriptorSetLayoutCreateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
            info.bindingCount = static_cast<std::uint32_t>(description.size());
            info.pBindings = description.data();
            Check(context.Function<PFN_vkCreateDescriptorSetLayout>("vkCreateDescriptorSetLayout")(context.device, &info, nullptr, &_layouts[stage]), "vkCreateDescriptorSetLayout");
        }
        Require(poolCounts[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER] <= context.limits.maxDescriptorSetUniformBuffers && poolCounts[VK_DESCRIPTOR_TYPE_STORAGE_BUFFER] <= context.limits.maxDescriptorSetStorageBuffers, "pipeline descriptors exceed device limits");
        std::vector<VkDescriptorPoolSize> sizes;
        for (const auto& [type, count] : poolCounts) {
            if (count != 0) sizes.push_back({type, count});
        }
        VkDescriptorPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        poolInfo.maxSets = static_cast<std::uint32_t>(shaders.size());
        poolInfo.poolSizeCount = static_cast<std::uint32_t>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();
        Check(context.Function<PFN_vkCreateDescriptorPool>("vkCreateDescriptorPool")(context.device, &poolInfo, nullptr, &pool), "vkCreateDescriptorPool");
        VkDescriptorSetAllocateInfo allocation{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocation.descriptorPool = pool;
        allocation.descriptorSetCount = static_cast<std::uint32_t>(shaders.size());
        allocation.pSetLayouts = _layouts.data();
        Check(context.Function<PFN_vkAllocateDescriptorSets>("vkAllocateDescriptorSets")(context.device, &allocation, _sets.data()), "vkAllocateDescriptorSets");
        for (std::size_t stage = 0; stage < bindings.size(); ++stage) {
            for (const auto& binding : bindings[stage]) {
                const auto& resource = allocations[binding.allocation];
                VkDescriptorBufferInfo buffer{resource.buffer->Handle(), 0, resource.size};
                VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                write.dstSet = _sets[stage];
                write.dstBinding = binding.layout.binding;
                write.descriptorCount = 1;
                write.descriptorType = binding.layout.descriptorType;
                write.pBufferInfo = &buffer;
                context.Function<PFN_vkUpdateDescriptorSets>("vkUpdateDescriptorSets")(context.device, 1, &write, 0, nullptr);
            }
        }
    } catch (...) {
        release();
        throw;
    }
}

ShaderResources::~ShaderResources() {
    release();
}

void ShaderResources::release() noexcept {
    if (pool) context.Function<PFN_vkDestroyDescriptorPool>("vkDestroyDescriptorPool")(context.device, pool, nullptr);
    for (auto layout : _layouts) {
        if (layout) context.Function<PFN_vkDestroyDescriptorSetLayout>("vkDestroyDescriptorSetLayout")(context.device, layout, nullptr);
    }
}

const std::vector<VkDescriptorSetLayout>& ShaderResources::Layouts() const {
    return _layouts;
}

void ShaderResources::Bind(VkCommandBuffer commands, VkPipelineLayout layout) const {
    context.Function<PFN_vkCmdBindDescriptorSets>("vkCmdBindDescriptorSets")(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, static_cast<std::uint32_t>(_sets.size()), _sets.data(), 0, nullptr);
}

void ShaderResources::WriteBack() {
    for (const auto& allocation : allocations) {
        if (allocation.writable) GuestMemory::CheckRange(reinterpret_cast<const void*>(allocation.address), allocation.size, 1, true);
    }
    for (const auto& allocation : allocations) {
        if (allocation.writable) GuestMemory::Write(allocation.address, allocation.buffer->Bytes());
    }
}

}
