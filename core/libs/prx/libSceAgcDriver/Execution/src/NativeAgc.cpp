#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgc/Shader/include/ShaderUtils.hpp"
#include "prx/libSceAgc/Shader/include/ShaderConstants.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeGraphicsState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeRegisterBindings.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeDrawCompiler.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeShaderArguments.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include <mutex>
#include <algorithm>
#include <cstring>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>
#include <unordered_map>
namespace Graphics = AgcDriver::Graphics;
namespace {
struct NativeShader {
    std::uint64_t identity;
    std::uint8_t type;
    std::uint64_t codeAddress;
    std::uint64_t headerAddress;
    std::vector<std::uint32_t> code;
    std::vector<std::byte> header;
    std::optional<ShaderSpecialRegs> specials;
    std::vector<ShaderRegister> registers;
    std::vector<ShaderRegister> contextRegisters;
    std::vector<ShaderSemantic> inputSemantics;
    std::vector<ShaderSemantic> outputSemantics;
    std::optional<ShaderUserData> userDataInfo;
};

struct NativeFlip {
    std::uintptr_t begin;
    std::uintptr_t end;
    AgcDriver::FlipInfo info;
};

struct NativeCompletion {
    std::uintptr_t begin;
    std::uintptr_t end;
    volatile std::uint32_t* address;
    std::uint32_t value;
};

struct NativeDrawCall {
    std::uintptr_t begin;
    std::uintptr_t end;
    Graphics::State graphics;
    Graphics::NativeGraphicsState baseGraphics;
    Graphics::NativeRegisterBindings context;
    Graphics::NativeRegisterBindings shaderBindings;
    Graphics::NativeRegisterBindings userBindings;
    std::shared_ptr<const NativeShader> vertexShader;
    std::shared_ptr<const NativeShader> fragmentShader;
    Graphics::NativeArgumentSnapshot vertexArguments;
    Graphics::NativeArgumentSnapshot fragmentArguments;
    Graphics::DrawParameters draw{};
    std::uint32_t indexOffset = 0;
    ShaderRecompiler::ShaderPixelStageInfo pixel{};
};

struct NativeCommandBufferState {
    std::uintptr_t recordingBegin = 0;
    std::uintptr_t recordingEnd = 0;
    Graphics::NativeGraphicsState graphics;
    Graphics::NativeRegisterBindings context;
    Graphics::NativeRegisterBindings shaderBindings;
    Graphics::NativeRegisterBindings userBindings;
    std::shared_ptr<const NativeShader> vertexShader;
    std::shared_ptr<const NativeShader> fragmentShader;
    std::shared_ptr<const NativeShader> computeShader;
    Graphics::NativeShaderArguments vertexArguments;
    Graphics::NativeShaderArguments fragmentArguments;
    std::uint32_t firstVertex = 0;
    std::uint64_t indexBuffer = 0;
    std::uint32_t indexCount = 0;
    std::uint8_t indexSize = 0;
    std::uint32_t instances = 1;
    std::vector<NativeDrawCall> draws;
    std::vector<NativeCompletion> completions;
    std::vector<NativeFlip> flips;
};
struct NativeRenderingWait {
    std::uintptr_t begin;
    std::uintptr_t end;
    std::uint32_t handle;
    std::uint32_t index;
};
std::map<std::uintptr_t, NativeRenderingWait> renderingWaits;
std::mutex submissionMutex;
std::mutex stateMutex;
std::map<std::pair<std::uintptr_t, std::uintptr_t>, NativeCommandBufferState> states;
std::unordered_map<const Shader*, std::shared_ptr<const NativeShader>> shaders;
std::uint64_t nextShaderIdentity = 0;
std::shared_ptr<const NativeShader> findShaderByProgramAddress(std::uint64_t address) {
    for (const auto& [header, shader] : shaders) {
        if (shader->codeAddress == address) return shader;
    }
    throw std::runtime_error("native AGC: shader program address does not reference a native shader");
}
void bindShaderRegister(NativeCommandBufferState& target, std::uint32_t offset, std::uint32_t value, const std::optional<std::uint32_t>& nextValue) {
    const auto bind=[&](std::shared_ptr<const NativeShader>& slot) {
        if (!nextValue) throw std::runtime_error("native AGC: incomplete shader program address");
        const auto address=(static_cast<std::uint64_t>(*nextValue & 0xffu)<<40u)|(static_cast<std::uint64_t>(value)<<8u);
        slot=findShaderByProgramAddress(address);
    };
    switch(offset) {
        case ShaderRegs::SPI_SHADER_PGM_LO_PS: bind(target.fragmentShader); return;
        case ShaderRegs::SPI_SHADER_PGM_LO_ES:
        case ShaderRegs::SPI_SHADER_PGM_LO_LS: bind(target.vertexShader); return;
        case ShaderRegs::COMPUTE_PGM_LO: bind(target.computeShader); return;
        default: throw std::runtime_error("native AGC: shader register has no native lowering");
    }
}
void writeShaderArgument(NativeCommandBufferState& target, std::uint32_t offset, std::uint32_t value) {
    if (offset == 0x8bu || offset == 0x0bu) {
        const auto count = ((value >> 1u) & 0x1fu) | (((value >> 27u) & 1u) << 5u);
        (offset == 0x8bu ? target.vertexArguments : target.fragmentArguments).SetCount(count);
    } else if (offset >= 0x8cu && offset < 0xacu) {
        target.vertexArguments.Write(offset - 0x8cu, std::span(&value, 1));
    } else if (offset >= 0x0cu && offset < 0x2cu) {
        target.fragmentArguments.Write(offset - 0x0cu, std::span(&value, 1));
    } else {
        throw std::runtime_error("native AGC: shader configuration has no native lowering at offset " + std::to_string(offset));
    }
}
void writeUserConfiguration(Graphics::NativeGraphicsState& graphics, std::uint8_t& indexSize,
                            std::uint32_t& firstVertex, std::uint32_t offset, std::uint32_t value) {
    // UC is graphics configuration, not the per-stage SH user-data arguments.
    switch (offset) {
        case 0x242u:
        case 0x25bu:
        case 0x262u: graphics.SetUser(offset, value); break;
        case 0x243u:
            if ((value & ~0x4c3u) != 0 || (value & 3u) > 2)
                throw std::invalid_argument("native AGC: invalid index configuration");
            indexSize = static_cast<std::uint8_t>(value & 3u); break;
        case 0x24au: firstVertex = value; break;
        case 0x24bu:
            if (value != 0) throw std::runtime_error("native AGC: primitive restart requires native lowering");
            break;
        default: throw std::runtime_error("native AGC: user configuration has no native lowering at offset " + std::to_string(offset));
    }
}
void prepareGraphics(NativeDrawCall& call, const Graphics::NativeGraphicsState& graphics) {
    if (!graphics.ReadyForDraw()) throw std::runtime_error("native AGC: draw graphics state is incomplete");
    call.graphics = graphics.Get();
    graphics.ValidateShaderContext(call.vertexShader->contextRegisters, call.fragmentShader->contextRegisters);
    const auto pixel=graphics.PixelStage();
    if(!pixel) throw std::runtime_error("native AGC: draw pixel-stage metadata is incomplete");
    call.pixel=*pixel;
    if (call.vertexShader->type != 2 || call.fragmentShader->type != 1 || !call.vertexShader->specials)
        throw std::runtime_error("native AGC: shader stages require an explicit native lowering");
    if (!graphics.MatchesVertexConfiguration(*call.vertexShader->specials))
        throw std::runtime_error("native AGC: vertex configuration differs from the compiled shader");
    const auto routing = call.vertexShader->specials->vgt_shader_stages_en.value;
    if ((routing & 0x2000u) == 0 || (routing & 0x24u) != 0 || call.graphics.rectList)
        throw std::runtime_error("native AGC: generated primitive stages require an explicit native lowering");
    call.graphics.stages = {Graphics::ShaderPath::Vertex, routing,
        (routing & 0x400000u) ? 32u : 64u, pixel->wave32 ? 32u : 64u, {}, {}};
}
void appendDraw(NativeCommandBufferState& s, CommandBuffer* buffer, std::uint32_t words,
                std::uint32_t count, bool indexed, std::uint64_t address, std::uint32_t firstVertex=0) {
    if (!s.flips.empty()) throw std::runtime_error("native AGC: drawing after a flip requires native lowering");
    if (std::any_of(s.completions.begin(), s.completions.end(), [](const auto& completion) { return completion.address != nullptr; }))
        throw std::runtime_error("native AGC: drawing after a terminal release requires native lowering");
    if (!s.shaderBindings.HasIndirect() && (!s.vertexShader || !s.fragmentShader)) throw std::runtime_error("native AGC: draw is missing native vertex or fragment shader");
    const auto bytes=s.indexSize==0?2u:s.indexSize==1?4u:0u;
    if(indexed && bytes==0 && !s.userBindings.HasIndirect()) throw std::runtime_error("native AGC: unsupported native index size");
    NativeDrawCall call;
    call.begin = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    call.end = call.begin + words * sizeof(std::uint32_t);
    call.baseGraphics = s.graphics;
    call.context = s.context;
    call.shaderBindings = s.shaderBindings;
    call.userBindings = s.userBindings;
    call.vertexShader=s.vertexShader;
    call.fragmentShader=s.fragmentShader;
    if (!s.shaderBindings.HasIndirect()) {
        call.vertexArguments=s.vertexArguments.Capture();
        call.fragmentArguments=s.fragmentArguments.Capture();
    }
    call.draw={indexed?address:0u,count,indexed?bytes:0u,s.instances,0u,indexed,firstVertex,0u};
    if (s.context.Empty() && !s.shaderBindings.HasIndirect() && !s.userBindings.HasIndirect()) prepareGraphics(call, s.graphics);
    s.draws.push_back(std::move(call));
}
std::uint64_t tokenCapacity(const CommandBuffer* buffer) {
    if (!buffer) throw std::invalid_argument("native AGC: null command buffer");
    const auto bottom = reinterpret_cast<std::uintptr_t>(buffer->bottom);
    const auto top = reinterpret_cast<std::uintptr_t>(buffer->top);
    const auto up = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    const auto down = reinterpret_cast<std::uintptr_t>(buffer->cursor_down);
    if ((!bottom && top) || ((bottom | top | up | down) & 3u) != 0 ||
        bottom > up || up > down || down > top)
        throw std::invalid_argument("native AGC: invalid command buffer storage or cursors");
    const auto available = (down - up) / sizeof(std::uint32_t);
    if (buffer->reserved_dw > available)
        throw std::invalid_argument("native AGC: reserved space exceeds command buffer capacity");
    return available - buffer->reserved_dw;
}
void reserveTokens(CommandBuffer* buffer, std::uint32_t count) {
    if (tokenCapacity(buffer) >= count) return;
    if (!buffer->callback)
        throw std::runtime_error("native AGC: command token storage exhausted");
    if (count > UINT32_MAX - buffer->reserved_dw)
        throw std::overflow_error("native AGC: command storage allocation overflow");
    if (!buffer->callback(buffer, count + buffer->reserved_dw, buffer->user_data))
        throw std::runtime_error("native AGC: command storage allocation callback failed");
    if (tokenCapacity(buffer) < count)
        throw std::runtime_error("native AGC: command storage callback returned insufficient space");
}
NativeCommandBufferState& state(CommandBuffer* buffer) {
    if (tokenCapacity(buffer) == 0)
        throw std::runtime_error("native AGC: command token storage exhausted");
    const auto bottom = reinterpret_cast<std::uintptr_t>(buffer->bottom);
    const auto cursor = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    auto& result = states[{bottom, reinterpret_cast<std::uintptr_t>(buffer->top)}];
    if (cursor < result.recordingEnd) {
        if (cursor != bottom && cursor != result.recordingBegin)
            throw std::runtime_error("native AGC: partial command-buffer rewind requires native lowering");
        result = {};
    }
    if (result.recordingBegin == 0) result.recordingBegin = cursor;
    return result;
}
NativeCommandBufferState* submittedState(const Packet* packet) {
    if (!packet) throw std::invalid_argument("native AGC: null submission");
    if (packet->dw_num == 0) throw std::invalid_argument("native AGC: empty submission");
    const auto begin=reinterpret_cast<std::uintptr_t>(packet->addr);
    const auto bytes=static_cast<std::uint64_t>(packet->dw_num)*sizeof(std::uint32_t);
    if (!begin || bytes>UINTPTR_MAX-begin) throw std::invalid_argument("native AGC: invalid submission range");
    const auto end=begin+bytes;
    NativeCommandBufferState* match=nullptr;
    for (auto& [storage, native] : states) {
        if (begin >= storage.first && end <= storage.second) {
            if (match) throw std::runtime_error("native AGC: submission ambiguously belongs to multiple command buffers");
            match=&native;
        }
    }
    return match;
}
std::uint32_t* opaque(CommandBuffer* buffer, std::uint32_t count = 1) {
    if (tokenCapacity(buffer) < count)
        throw std::runtime_error("native AGC: command token storage exhausted");
    auto* token = buffer->cursor_up;
    std::fill_n(token, count, 0x80000000u);
    buffer->cursor_up += count;
    states.at({reinterpret_cast<std::uintptr_t>(buffer->bottom), reinterpret_cast<std::uintptr_t>(buffer->top)}).recordingEnd = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    return token;
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
    if (shader->specials) native->specials=*shader->specials;
    if (shader->sh_registers && shader->num_sh_registers)
        native->registers.assign(shader->sh_registers, shader->sh_registers + shader->num_sh_registers);
    if (shader->cx_registers && shader->num_cx_registers)
        native->contextRegisters.assign(shader->cx_registers, shader->cx_registers + shader->num_cx_registers);
    if (shader->input_semantics && shader->num_input_semantics)
        native->inputSemantics.assign(shader->input_semantics,shader->input_semantics+shader->num_input_semantics);
    if (shader->output_semantics && shader->num_output_semantics)
        native->outputSemantics.assign(shader->output_semantics,shader->output_semantics+shader->num_output_semantics);
    if (shader->user_data) native->userDataInfo=*shader->user_data;
    {
        std::lock_guard lock(stateMutex);
        shaders.insert_or_assign(shader, std::move(native));
    }
    *dst = shader;
    return 0;
}
static std::uint32_t* setIndirect(CommandBuffer* buffer, const volatile ShaderRegister* registers, std::uint32_t count,
    Graphics::NativeRegisterBindings NativeCommandBufferState::* member) {
    AgcDriver::NativeGraphicsRuntime::Get().CheckFailure();
    Graphics::NativeRegisterBindings binding;
    binding.Bind(registers, count);
    reserveTokens(buffer, 5);
    std::lock_guard lock(stateMutex);
    auto& target = state(buffer).*member;
    auto updated = target;
    updated.Merge(binding);
    auto* token = opaque(buffer, 5);
    target = std::move(updated);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetCxRegistersIndirect(CommandBuffer* buffer, const volatile ShaderRegister* registers, std::uint32_t count) {
    return setIndirect(buffer, registers, count, &NativeCommandBufferState::context);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegistersIndirect(CommandBuffer* buffer, const volatile ShaderRegister* registers, std::uint32_t count) {
    return setIndirect(buffer, registers, count, &NativeCommandBufferState::shaderBindings);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegistersIndirect(CommandBuffer* buffer, const volatile ShaderRegister* registers, std::uint32_t count) {
    return setIndirect(buffer, registers, count, &NativeCommandBufferState::userBindings);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisters(CommandBuffer* b, const volatile ShaderRegister* regs, std::uint32_t count) {
    if (!regs || count == 0 || count > 0x4000u) throw std::invalid_argument("native AGC: invalid shader argument list");
    std::lock_guard lock(stateMutex); auto& target=state(b);
    auto bindings = target.shaderBindings;
    for (std::uint32_t i=0;i<count;++i) bindings.Write(regs[i].offset, regs[i].value);
    for (std::uint32_t i=0;i<count;++i) {
        const std::optional<std::uint32_t> next=(i+1<count && regs[i+1].offset==regs[i].offset+1)?std::optional<std::uint32_t>(regs[i+1].value):std::nullopt;
        if (regs[i].offset==ShaderRegs::SPI_SHADER_PGM_LO_PS || regs[i].offset==ShaderRegs::SPI_SHADER_PGM_LO_ES || regs[i].offset==ShaderRegs::SPI_SHADER_PGM_LO_LS || regs[i].offset==ShaderRegs::COMPUTE_PGM_LO) {
            bindShaderRegister(target,regs[i].offset,regs[i].value,next); ++i;
        } else writeShaderArgument(target, regs[i].offset, regs[i].value);
    }
    auto* token = opaque(b);
    target.shaderBindings = std::move(bindings);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisterRange(CommandBuffer* b, std::uint32_t offset, const std::uint32_t* values, std::uint32_t count) {
    if (!values || (reinterpret_cast<std::uintptr_t>(values) & 3u) != 0 || count == 0 || count > 0x3fffu || offset > 0xffffu || count > 0x10000u - offset)
        throw std::invalid_argument("native AGC: invalid shader argument range");
    const std::vector<std::uint32_t> snapshot(values, values + count);
    reserveTokens(b, count + 2u);
    std::lock_guard lock(stateMutex); auto& target=state(b);
    auto bindings = target.shaderBindings;
    for (std::uint32_t i=0;i<count;++i) bindings.Write(offset + i, snapshot[i]);
    for (std::uint32_t i=0;i<count;++i) {
        const auto current=offset+i;
        if (current==ShaderRegs::SPI_SHADER_PGM_LO_PS || current==ShaderRegs::SPI_SHADER_PGM_LO_ES || current==ShaderRegs::SPI_SHADER_PGM_LO_LS || current==ShaderRegs::COMPUTE_PGM_LO) {
            const std::optional<std::uint32_t> next=i+1<count?std::optional<std::uint32_t>(snapshot[i+1]):std::nullopt;
            bindShaderRegister(target,current,snapshot[i],next); ++i;
        } else writeShaderArgument(target, current, snapshot[i]);
    }
    auto* token = opaque(b, count + 2u);
    target.shaderBindings = std::move(bindings);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisters(CommandBuffer* b, const volatile ShaderRegister* regs, std::uint32_t count) {
    if (!regs || count == 0 || count > 0x4000u)
        throw std::invalid_argument("native AGC: invalid user configuration list");
    std::lock_guard lock(stateMutex);
    auto& target = state(b);
    auto graphics = target.graphics;
    auto indexSize = target.indexSize;
    auto firstVertex = target.firstVertex;
    auto bindings = target.userBindings;
    for (std::uint32_t i = 0; i < count; ++i)
        writeUserConfiguration(graphics, indexSize, firstVertex, regs[i].offset, regs[i].value);
    for (std::uint32_t i = 0; i < count; ++i) bindings.Write(regs[i].offset, regs[i].value);
    auto* token = opaque(b);
    target.graphics = std::move(graphics);
    target.indexSize = indexSize;
    target.firstVertex = firstVertex;
    target.userBindings = std::move(bindings);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisterRange(CommandBuffer* b, std::uint32_t offset, const std::uint32_t* values, std::uint32_t count) {
    if (!values || (reinterpret_cast<std::uintptr_t>(values) & 3u) != 0 || count == 0 || count > 0x3fffu || offset > 0xffffu || count > 0x10000u - offset)
        throw std::invalid_argument("native AGC: invalid user configuration range");
    const std::vector<std::uint32_t> snapshot(values, values + count);
    reserveTokens(b, count + 2u);
    std::lock_guard lock(stateMutex);
    auto& target = state(b);
    auto graphics = target.graphics;
    auto indexSize = target.indexSize;
    auto firstVertex = target.firstVertex;
    auto bindings = target.userBindings;
    for (std::uint32_t i = 0; i < count; ++i)
        writeUserConfiguration(graphics, indexSize, firstVertex, offset + i, snapshot[i]);
    for (std::uint32_t i = 0; i < count; ++i) bindings.Write(offset + i, snapshot[i]);
    auto* token = opaque(b, count + 2u);
    target.graphics = std::move(graphics);
    target.indexSize = indexSize;
    target.firstVertex = firstVertex;
    target.userBindings = std::move(bindings);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexBuffer(CommandBuffer* b, std::uint64_t address) {
    if (!address || (address & 1u)) throw std::invalid_argument("native AGC: null or misaligned index buffer");
    reserveTokens(b, 3);
    std::lock_guard lock(stateMutex); auto& target = state(b);
    auto* token = opaque(b, 3);
    target.indexBuffer = address;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexCount(CommandBuffer* b, std::uint32_t count) {
    reserveTokens(b, 2);
    std::lock_guard lock(stateMutex); auto& target = state(b);
    auto* token = opaque(b, 2);
    target.indexCount = count;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexSize(CommandBuffer* b, std::uint8_t size, std::uint8_t cachePolicy) {
    if (size > 2 || cachePolicy > 3) throw std::invalid_argument("native AGC: invalid index configuration");
    reserveTokens(b, 3);
    std::lock_guard lock(stateMutex); auto& target = state(b);
    auto* token = opaque(b, 3);
    target.indexSize = size;
    target.userBindings.Write(0x243u, size);
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcSetNumInstances(CommandBuffer* b, std::uint32_t count) {
    reserveTokens(b, 2);
    std::lock_guard lock(stateMutex); auto& target = state(b);
    auto* token = opaque(b, 2);
    target.instances = count;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndex(CommandBuffer* b, std::uint32_t count, const volatile void* address, std::uint64_t modifier) {
    if (!address) throw std::invalid_argument("native AGC: null draw index address");
    if (modifier != 0) throw std::invalid_argument("native AGC: draw modifier requires native lowering");
    reserveTokens(b, 6);
    std::lock_guard lock(stateMutex);
    auto& target = state(b);
    const auto indexAddress = reinterpret_cast<std::uintptr_t>(address);
    appendDraw(target, b, 6, count, true, indexAddress);
    auto* token = opaque(b, 6);
    target.indexBuffer = indexAddress;
    target.indexCount = count;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexAuto(CommandBuffer* b, std::uint32_t count, std::uint64_t modifier) {
    if (modifier != 0 && modifier != 2) throw std::invalid_argument("native AGC: draw modifier requires native lowering");
    reserveTokens(b, 3);
    std::lock_guard lock(stateMutex);
    auto& target = state(b);
    appendDraw(target, b, 3, count, false, 0, target.firstVertex);
    auto* token = opaque(b, 3);
    target.indexCount = count;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexOffset(CommandBuffer* b, std::uint32_t offset, std::uint32_t count, std::uint64_t modifier) {
    if (modifier != 0) throw std::invalid_argument("native AGC: draw modifier requires native lowering");
    reserveTokens(b, 5);
    std::lock_guard lock(stateMutex);
    auto& target = state(b);
    const auto bytes = target.indexSize == 0 ? 2u : target.indexSize == 1 ? 4u : 0u;
    if (!target.indexBuffer || (!bytes && !target.userBindings.HasIndirect()))
        throw std::runtime_error("native AGC: indexed offset draw is missing index buffer state");
    const auto displacement = static_cast<std::uint64_t>(offset) * bytes;
    if (displacement > UINT64_MAX - target.indexBuffer)
        throw std::overflow_error("native AGC: indexed offset address overflow");
    appendDraw(target, b, 5, count, true, target.indexBuffer);
    target.draws.back().indexOffset = offset;
    if (!target.userBindings.HasIndirect()) target.draws.back().draw.indexAddress += displacement;
    auto* token = opaque(b, 5);
    target.indexCount = count;
    return token;
}
std::uint32_t* APS5_VABI aps5NativeAgcReleaseMem(CommandBuffer* buffer, std::uint8_t action,
    std::uint16_t gcrControl, std::uint8_t destination, std::uint8_t cachePolicy,
    const volatile Label* label, std::uint8_t dataSelect, std::uint64_t data,
    std::uint16_t, std::uint16_t, std::uint8_t interrupt, std::uint32_t interruptContextId) {
    AgcDriver::NativeGraphicsRuntime::Get().CheckFailure();
    const bool cacheRelease = action == 0x2du && destination == 1 && dataSelect == 0;
    const bool completion = action == 0x28u && destination == 0 && dataSelect <= 1;
    if ((!cacheRelease && !completion) || (gcrControl & ~0x30cu) != 0 || cachePolicy != 0 ||
        interrupt != 0 || interruptContextId != 0)
        throw std::invalid_argument("native AGC: release operation requires native lowering");
    const auto address = reinterpret_cast<std::uintptr_t>(label);
    if (dataSelect == 1 && (!address || (address & 3u) != 0 || data > UINT32_MAX))
        throw std::invalid_argument("native AGC: invalid completion marker");
    reserveTokens(buffer, 8);
    std::lock_guard lock(stateMutex);
    auto& target = state(buffer);
    const auto begin = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    target.completions.push_back({begin, begin + 8 * sizeof(std::uint32_t),
        dataSelect == 1 ? reinterpret_cast<volatile std::uint32_t*>(address) : nullptr,
        static_cast<std::uint32_t>(data)});
    return opaque(buffer, 8);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetFlip(CommandBuffer* buffer, std::uint32_t handle,
    std::int32_t index, std::uint32_t mode, std::int64_t argument) {
    AgcDriver::NativeGraphicsRuntime::Get().CheckFailure();
    reserveTokens(buffer, AgcDriver::FlipPacketWords);
    std::lock_guard lock(stateMutex);
    auto& target = state(buffer);
    const auto begin = reinterpret_cast<std::uintptr_t>(buffer->cursor_up);
    target.flips.push_back({begin, begin + AgcDriver::FlipPacketWords * 4, {handle, index, mode, argument}});
    return opaque(buffer, AgcDriver::FlipPacketWords);
}
std::uint32_t APS5_VABI aps5NativeAgcGetWaitRenderingSize() {
    return AgcDriver::RenderingWaitPacketWords;
}
std::uint32_t APS5_VABI aps5NativeAgcWaitUntilSafeForRendering(std::uint32_t** command,
    std::uint32_t capacity, std::uint32_t mode, std::uint32_t handle, int index) {
    AgcDriver::NativeGraphicsRuntime::Get().CheckFailure();
    if (!command || capacity < AgcDriver::RenderingWaitPacketWords || mode != 0 || index < 0)
        throw std::invalid_argument("native AGC: invalid rendering wait arguments");
    AgcDriver::GuestMemory::CheckRange(command, sizeof(*command), alignof(std::uint32_t*), true);
    AgcDriver::GuestMemory::CheckRange(*command, AgcDriver::RenderingWaitPacketWords * 4, 4, true);
    const auto begin = reinterpret_cast<std::uintptr_t>(*command);
    std::lock_guard lock(stateMutex);
    renderingWaits.insert_or_assign(begin, NativeRenderingWait{begin,
        begin + AgcDriver::RenderingWaitPacketWords * 4, handle, static_cast<std::uint32_t>(index)});
    std::fill_n(*command, AgcDriver::RenderingWaitPacketWords, 0x80000000u);
    *command += AgcDriver::RenderingWaitPacketWords;
    return 0;
}
int APS5_VABI aps5NativeAgcSubmit(const Packet* packet) {
    const auto received = AgcDriver::FrameTiming::Clock::now();
    auto& runtime = AgcDriver::NativeGraphicsRuntime::Get();
    runtime.CheckFailure();
    std::lock_guard submissionLock(submissionMutex);
    std::vector<NativeRenderingWait> waits;
    std::vector<NativeDrawCall> draws;
    std::vector<NativeCompletion> completions;
    std::vector<NativeFlip> flips;
    {
        std::lock_guard lock(stateMutex);
        auto* native = submittedState(packet);
        const auto begin = reinterpret_cast<std::uintptr_t>(packet->addr);
        const auto end = begin + static_cast<std::uint64_t>(packet->dw_num) * sizeof(std::uint32_t);
        const auto covered = [&](const auto& operation) { return operation.begin >= begin && operation.end <= end; };
        for (const auto& [address, wait] : renderingWaits) {
            if (wait.begin >= end || wait.end <= begin) continue;
            if (!covered(wait)) throw std::runtime_error("native AGC: submission splits a rendering wait");
            if (native) {
                for (const auto& draw : native->draws)
                    if (wait.end > draw.begin) throw std::runtime_error("native AGC: rendering wait after a draw requires native lowering");
                for (const auto& flip : native->flips)
                    if (wait.end > flip.begin) throw std::runtime_error("native AGC: rendering wait after a flip requires native lowering");
                for (const auto& completion : native->completions)
                    if (wait.end > completion.begin) throw std::runtime_error("native AGC: rendering wait after a release requires native lowering");
            }
            waits.push_back(wait);
        }
        if (!native && waits.empty())
            throw std::runtime_error("native AGC: submission was not authored by a lowered native command buffer");
        if (native && (!std::all_of(native->draws.begin(), native->draws.end(), covered) ||
            !std::all_of(native->completions.begin(), native->completions.end(), covered) ||
            !std::all_of(native->flips.begin(), native->flips.end(), covered)))
            throw std::runtime_error("native AGC: partial submission excludes pending native work");
        if (native) {
            draws = std::move(native->draws);
            completions = std::move(native->completions);
            flips = std::move(native->flips);
            native->draws.clear();
            native->completions.clear();
            native->flips.clear();
        }
        for (const auto& wait : waits) renderingWaits.erase(wait.begin);
    }
    std::vector<std::shared_ptr<AgcDriver::IFlipRequest>> requests;
    try {
        requests.reserve(flips.size());
        std::vector<std::shared_ptr<AgcDriver::IRenderingWait>> dependencies;
        for (const auto& wait : waits)
            dependencies.push_back(AgcDriver::CaptureNativeRenderingWait(wait.handle, wait.index));
        for (const auto& flip : flips) requests.push_back(AgcDriver::ReserveNativeFlip(flip.info));
        for (const auto& dependency : dependencies) dependency->Wait();
        {
            std::lock_guard gpuLock(runtime.Mutex());
            runtime.CheckFailure();
            for(auto& call:draws){
                if (!call.context.Empty() || call.shaderBindings.HasIndirect() || call.userBindings.HasIndirect()) {
                    auto graphics = call.baseGraphics;
                    if (call.userBindings.HasIndirect()) {
                        std::uint8_t indexSize = call.draw.indexSize == 4 ? 1 : call.draw.indexSize == 2 ? 0 : 2;
                        std::uint32_t firstVertex = call.draw.firstVertex;
                        for (const auto& [offset, value] : call.userBindings.Resolve())
                            writeUserConfiguration(graphics, indexSize, firstVertex, offset, value);
                        if (call.draw.indexed) {
                            if (indexSize > 1) throw std::runtime_error("native AGC: unsupported native index size");
                            call.draw.indexSize = indexSize == 0 ? 2 : 4;
                            const auto displacement = static_cast<std::uint64_t>(call.indexOffset) * call.draw.indexSize;
                            if (displacement > UINT64_MAX - call.draw.indexAddress)
                                throw std::overflow_error("native AGC: indexed offset address overflow");
                            call.draw.indexAddress += displacement;
                        } else call.draw.firstVertex = firstVertex;
                    }
                    if (call.shaderBindings.HasIndirect()) {
                        const auto registers = call.shaderBindings.Resolve();
                        NativeCommandBufferState shaderState;
                        std::lock_guard lock(stateMutex);
                        for (auto reg = registers.begin(); reg != registers.end(); ++reg) {
                            const auto offset = reg->first;
                            if (offset == ShaderRegs::SPI_SHADER_PGM_LO_PS || offset == ShaderRegs::SPI_SHADER_PGM_LO_ES ||
                                offset == ShaderRegs::SPI_SHADER_PGM_LO_LS || offset == ShaderRegs::COMPUTE_PGM_LO) {
                                const auto next = std::next(reg);
                                const std::optional<std::uint32_t> high = next != registers.end() && next->first == offset + 1 ?
                                    std::optional<std::uint32_t>(next->second) : std::nullopt;
                                bindShaderRegister(shaderState, offset, reg->second, high);
                                ++reg;
                            }
                        }
                        for (const auto& [offset, value] : registers) {
                            if (offset == ShaderRegs::SPI_SHADER_PGM_LO_PS || offset == ShaderRegs::SPI_SHADER_PGM_LO_PS + 1 ||
                                offset == ShaderRegs::SPI_SHADER_PGM_LO_ES || offset == ShaderRegs::SPI_SHADER_PGM_LO_ES + 1 ||
                                offset == ShaderRegs::SPI_SHADER_PGM_LO_LS || offset == ShaderRegs::SPI_SHADER_PGM_LO_LS + 1 ||
                                offset == ShaderRegs::COMPUTE_PGM_LO || offset == ShaderRegs::COMPUTE_PGM_LO + 1) continue;
                            const bool arguments = offset == 0x8bu || offset == 0x0bu ||
                                (offset >= 0x8cu && offset < 0xacu) || (offset >= 0x0cu && offset < 0x2cu);
                            if (arguments) { writeShaderArgument(shaderState, offset, value); continue; }
                            const auto declared = [&](const std::shared_ptr<const NativeShader>& shader) {
                                return shader && std::any_of(shader->registers.begin(), shader->registers.end(),
                                    [&](const auto& item) { return item.offset == offset && item.value == value; });
                            };
                            if (!declared(shaderState.vertexShader) && !declared(shaderState.fragmentShader))
                                throw std::runtime_error("native AGC: shader configuration differs from compiled metadata at offset " + std::to_string(offset));
                        }
                        if (!shaderState.vertexShader || !shaderState.fragmentShader)
                            throw std::runtime_error("native AGC: draw is missing native vertex or fragment shader");
                        call.vertexShader = shaderState.vertexShader;
                        call.fragmentShader = shaderState.fragmentShader;
                        call.vertexArguments = shaderState.vertexArguments.Capture();
                        call.fragmentArguments = shaderState.fragmentArguments.Capture();
                    }
                    for (const auto& [offset, value] : call.context.Resolve()) graphics.SetContext(offset, value);
                    prepareGraphics(call, graphics);
                }
                auto& nativeDevice = runtime.Headless();
                const auto makeBinary=[](const NativeShader& shader, ShaderRecompiler::ShaderStage stage){
                    return ShaderRecompiler::ShaderBinary{stage,shader.codeAddress,shader.code,shader.headerAddress,shader.header,shader.identity};
                };
                std::array<Graphics::NativeShaderProgram,2> programs{{
                    {makeBinary(*call.vertexShader,ShaderRecompiler::ShaderStage::Vertex),ShaderRecompiler::ProgramRole::Main,0x8cu,8,call.vertexArguments.values,call.vertexArguments.missing},
                    {makeBinary(*call.fragmentShader,ShaderRecompiler::ShaderStage::Fragment),ShaderRecompiler::ProgramRole::Fragment,0x0cu,0,call.fragmentArguments.values,call.fragmentArguments.missing}
                }};
                std::array<ShaderRecompiler::MemoryRegion,4> memory{{
                    {call.vertexShader->codeAddress,std::as_bytes(std::span(call.vertexShader->code))},
                    {call.vertexShader->headerAddress,call.vertexShader->header},
                    {call.fragmentShader->codeAddress,std::as_bytes(std::span(call.fragmentShader->code))},
                    {call.fragmentShader->headerAddress,call.fragmentShader->header}
                }};
                Graphics::CompileAndEnqueueNativeDraw(nativeDevice,call.graphics,call.draw,programs,call.pixel,memory);
            }
            runtime.WaitDraws();
        }
        auto completion = completions.begin();
        for (std::size_t i = 0; i < flips.size(); ++i) {
            while (completion != completions.end() && completion->end <= flips[i].begin) {
                if (completion->address) *completion->address = completion->value;
                ++completion;
            }
            requests[i]->GpuReady(AgcDriver::FrameTiming::NativeFlip(received));
        }
        for (; completion != completions.end(); ++completion)
            if (completion->address) *completion->address = completion->value;
    } catch (...) {
        const auto failure = std::current_exception();
        for (const auto& request : requests) request->Fail(failure);
        AgcDriverReportFailure_nid_postfix(failure);
        throw;
    }
    return 0;
}
}
