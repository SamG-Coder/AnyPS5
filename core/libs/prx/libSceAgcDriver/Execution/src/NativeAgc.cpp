#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgc/Shader/include/ShaderUtils.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeGraphicsState.hpp"
#include <mutex>
#include <cstring>
#include <memory>
#include <vector>
#include <stdexcept>
#include <unordered_map>
namespace {
struct NativeShader {
    std::uint64_t identity;
    std::uint8_t type;
    std::uint64_t codeAddress;
    std::uint64_t headerAddress;
    std::vector<std::uint32_t> code;
    std::vector<std::byte> header;
};

struct NativeCommandBufferState {
    Graphics::NativeGraphicsState graphics;
    std::vector<ShaderRegister> shaderBindings;
    std::uint64_t indexBuffer = 0;
    std::uint32_t indexCount = 0;
    std::uint8_t indexSize = 0;
    std::uint32_t instances = 1;
};
std::mutex stateMutex;
std::unordered_map<CommandBuffer*, NativeCommandBufferState> states;
std::unordered_map<const Shader*, std::shared_ptr<const NativeShader>> shaders;
std::uint64_t nextShaderIdentity = 0;
NativeCommandBufferState& state(CommandBuffer* buffer) {
    if (!buffer) throw std::invalid_argument("native AGC: null command buffer");
    return states[buffer];
}
std::uint32_t* opaque(CommandBuffer* buffer) {
    // The lowered ABI still returns a command handle because original code may
    // retain the value. It is not executable PS5 PM4 and is never interpreted.
    return buffer ? buffer->cursor_up : nullptr;
}
}
extern "C" {
int APS5_VABI aps5NativeAgcCreateShader(Shader** dst, void* header, const volatile void* code) {
    if (!dst || !header || !code) throw std::invalid_argument("native AGC: invalid shader creation arguments");
    auto* shader = static_cast<Shader*>(header);
    if (shader->file_header != 0x34333231u || shader->version != 0x18u)
        throw std::invalid_argument("native AGC: invalid shader header");
    if (shader->header_size < sizeof(Shader) || shader->shader_size == 0 || (shader->shader_size & 3u) != 0)
        throw std::invalid_argument("native AGC: invalid shader size");
    ResolveRelativePtr(shader->cx_registers);
    ResolveRelativePtr(shader->sh_registers);
    ResolveRelativePtr(shader->user_data);
    ResolveRelativePtr(shader->specials);
    ResolveRelativePtr(shader->input_semantics);
    ResolveRelativePtr(shader->output_semantics);
    if (shader->user_data) {
        ResolveRelativePtr(shader->user_data->direct_resource_offset);
        for (auto& item : shader->user_data->sharp_resource_offset) ResolveRelativePtr(item);
    }
    shader->code = code;
    const auto base = reinterpret_cast<std::uint64_t>(code);
    if ((base & 0xffu) != 0) throw std::invalid_argument("native AGC: shader code is not 256-byte aligned");
    const auto patch = PatchProgramAddressRegister(shader->sh_registers, shader->num_sh_registers, shader->type, base);
    if (patch != 0) return patch;
    auto native = std::make_shared<NativeShader>();
    {
        std::lock_guard lock(stateMutex);
        if (nextShaderIdentity == UINT64_MAX) throw std::overflow_error("native AGC: shader identity overflow");
        native->identity = ++nextShaderIdentity;
    }
    native->type = shader->type;
    native->codeAddress = reinterpret_cast<std::uintptr_t>(code);
    native->headerAddress = reinterpret_cast<std::uintptr_t>(shader);
    native->code.resize(shader->shader_size / sizeof(std::uint32_t));
    std::memcpy(native->code.data(), const_cast<const void*>(code), shader->shader_size);
    native->header.resize(shader->header_size);
    std::memcpy(native->header.data(), shader, shader->header_size);
    {
        std::lock_guard lock(stateMutex);
        shaders.insert_or_assign(shader, std::move(native));
    }
    *dst = shader;
    return 0;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisters(CommandBuffer* b, const volatile ShaderRegister* regs, std::uint32_t count) {
    if (!regs && count) throw std::invalid_argument("native AGC: null shader register list");
    std::lock_guard lock(stateMutex); auto& target=state(b).shaderBindings;
    target.reserve(target.size()+count);
    for (std::uint32_t i=0;i<count;++i) target.push_back({regs[i].offset,regs[i].value});
    return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisterRange(CommandBuffer* b, std::uint32_t offset, const std::uint32_t* values, std::uint32_t count) {
    std::lock_guard lock(stateMutex); auto& target=state(b).shaderBindings;
    if (values) { target.reserve(target.size()+count); for (std::uint32_t i=0;i<count;++i) target.push_back({offset+i,values[i]}); }
    return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisters(CommandBuffer* b, const volatile ShaderRegister* regs, std::uint32_t count) {
    if (!regs && count) throw std::invalid_argument("native AGC: null user register list");
    std::lock_guard lock(stateMutex); auto& target=state(b).graphics;
    for (std::uint32_t i=0;i<count;++i) target.SetUser(regs[i].offset, regs[i].value);
    return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisterRange(CommandBuffer* b, std::uint32_t offset, const std::uint32_t* values, std::uint32_t count) {
    std::lock_guard lock(stateMutex); auto& target=state(b).graphics;
    if (values) for (std::uint32_t i=0;i<count;++i) target.SetUser(offset+i, values[i]);
    return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexBuffer(CommandBuffer* b, std::uint64_t address) {
    if (!address) throw std::invalid_argument("native AGC: null index buffer");
    std::lock_guard lock(stateMutex); state(b).indexBuffer = address; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexCount(CommandBuffer* b, std::uint32_t count) {
    std::lock_guard lock(stateMutex); state(b).indexCount = count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexSize(CommandBuffer* b, std::uint8_t size, std::uint8_t) {
    if (size > 2) throw std::invalid_argument("native AGC: invalid index size");
    std::lock_guard lock(stateMutex); state(b).indexSize = size; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetNumInstances(CommandBuffer* b, std::uint32_t count) {
    std::lock_guard lock(stateMutex); state(b).instances = count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndex(CommandBuffer* b, std::uint32_t count, const volatile void* address, std::uint64_t) {
    if (!address) throw std::invalid_argument("native AGC: null draw index address");
    std::lock_guard lock(stateMutex); auto& s=state(b); s.indexBuffer=reinterpret_cast<std::uintptr_t>(address); s.indexCount=count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexAuto(CommandBuffer* b, std::uint32_t count, std::uint64_t) {
    std::lock_guard lock(stateMutex); state(b).indexCount=count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexOffset(CommandBuffer* b, std::uint32_t, std::uint32_t count, std::uint64_t) {
    std::lock_guard lock(stateMutex); state(b).indexCount=count; return opaque(b);
}
int APS5_VABI aps5NativeAgcSubmit(const Packet*) {
    throw std::runtime_error("native AGC submission reached before native Vulkan lowering is complete");
}
}
