#include "prx/libSceAgcDriver/Graphics/include/ShaderResources.hpp"
#include <array>
#include <bit>
#include <iostream>
#include <string_view>

namespace {

using AgcDriver::Graphics::Require;

alignas(256) std::array<std::byte, 1024> colorMemory{};

AgcDriver::QueueState makeState() {
    AgcDriver::QueueState queue;
    queue.userConfig[0x242] = 4;
    queue.context = {
        {0x2d5, 0x2000},
        {0x1b6, 0}, {0x207, 0}, {0x200, 0}, {0x203, 0x800},
        {0x2dc, 0xaa00}, {0x2f8, 0}, {0x292, 2}, {0x293, 0},
        {0x80, 0}, {0x8d, 0}, {0x83, 0xffff}, {0x8c, 0xa},
        {0x2f9, 0x2d}, {0x313, 0x6000}, {0x30e, 0xffffffff}, {0x30f, 0xffffffff},
        {0x206, 0x43f}, {0x204, 0x80000}, {0x205, 0x240},
        {0x8e, 0xf}, {0x8f, 0xf}, {0x202, 0xcc0010},
        {0x1c4, 0}, {0x1c5, 9}, {0x1c3, 4}, {0x31c, 0x28028},
        {0x31b, 0}, {0x31d, 0}, {0x3b0, (63u << 14u) | 3u},
        {0x3b8, 0x9000000}, {0x1e0, 0},
        {0xc, 0}, {0xd, 0x40040},
        {0x81, 0x80000000}, {0x82, 0x40040},
        {0x90, 0x80000000}, {0x91, 0x40040},
        {0x94, 0x80000000}, {0x95, 0x40040}
    };
    const auto address = reinterpret_cast<std::uintptr_t>(colorMemory.data());
    queue.context[0x318] = static_cast<std::uint32_t>(address >> 8u);
    queue.context[0x390] = static_cast<std::uint32_t>(address >> 40u);
    queue.context[0x10f] = std::bit_cast<std::uint32_t>(32.0f);
    queue.context[0x110] = std::bit_cast<std::uint32_t>(32.0f);
    queue.context[0x111] = std::bit_cast<std::uint32_t>(-2.0f);
    queue.context[0x112] = std::bit_cast<std::uint32_t>(2.0f);
    queue.context[0x113] = std::bit_cast<std::uint32_t>(1.0f);
    queue.context[0x114] = 0;
    queue.context[0xb4] = 0;
    queue.context[0xb5] = std::bit_cast<std::uint32_t>(1.0f);
    return queue;
}

template<typename TAction>
void expectFailure(TAction action, std::string_view reason) {
    try {
        action();
    } catch (const std::runtime_error& error) {
        Require(std::string_view(error.what()).find(reason) != std::string_view::npos, std::string("unexpected failure: ") + error.what());
        return;
    }
    throw std::runtime_error("expected graphics rejection: " + std::string(reason));
}

void stateTests() {
    AgcDriver::QueueState initial;
    Require(initial.context.at(0x200) == 0 && initial.context.at(0x83) == 0xffff, "initial context state is missing");
    initial.context[0x200] = 7;
    initial.context[0xdead] = 1;
    initial.ClearContext();
    Require(initial.context.at(0x200) == 0 && !initial.context.contains(0xdead), "context reset did not restore defaults");
    Require(initial.userConfig.at(0x24b) == 0, "primitive restart must be disabled in initial queue state");
    initial.userConfig[0x24b] = 1;
    initial.ClearContext();
    Require(initial.userConfig.at(0x24b) == 1, "context clear must preserve user configuration");
    initial = AgcDriver::QueueState{};
    Require(initial.userConfig.at(0x24b) == 0, "queue reset must disable primitive restart");
    auto queue = makeState();
    auto state = AgcDriver::Graphics::DecodeState(queue);
    Require(state.color.address == reinterpret_cast<std::uintptr_t>(colorMemory.data()) && state.color.bytes == colorMemory.size(), "render-target address or size changed");
    Require(state.viewport.y == 4 && state.viewport.height == -4, "negative viewport height was lost");
    Require(state.color.format == VK_FORMAT_R8G8B8A8_UNORM, "RGBA format changed");
    queue.userConfig[0x24b] = 1;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "GE_MULTI_PRIM_IB_RESET_EN");
    queue = makeState();
    queue.userConfig.erase(0x24b);
    queue.context[0x2a5] = 0;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "user-config bank at DWORD 0x24b");
    queue = makeState();
    queue.context[0x90] = 0x80010003;
    queue.context[0x91] = 0x30020;
    state = AgcDriver::Graphics::DecodeState(queue);
    Require(state.scissor.offset.x == 3 && state.scissor.offset.y == 1 && state.scissor.extent.width == 29 && state.scissor.extent.height == 2, "scissor intersection changed");
    queue.context[0x31c] |= 0x10000000;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "DCC");
    queue = makeState();
    queue.context.erase(0x3b8);
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "missing register");
    queue = makeState();
    queue.context[0x3b8] |= 27u << 14u;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "only linear");
    queue = makeState();
    queue.context[0x3b0] = (62u << 14u) | 3u;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "pitch");
    queue = makeState();
    queue.context[0x8e] = 0xff;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "target zero");
    queue = makeState();
    queue.context[0x200] = 2;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "depth");
    queue = makeState();
    queue.context[0x10f] = 0x7fc00000;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "non-finite");
}

void ShaderStageTests() {
    auto queue = makeState();
    for (const auto routing : {0x2000u, 0x2010u, 0x02002000u, 0x02002010u}) {
        for (const auto vertexWave32 : {false, true}) {
            for (const auto fragmentWave32 : {false, true}) {
                queue.context[0x2d5] = routing | (vertexWave32 ? 0x00400000u : 0u);
                queue.context[0x1b6] = fragmentWave32 ? 0x8000u : 0u;
                const auto state = AgcDriver::Graphics::DecodeState(queue);
                Require(state.stages.path == AgcDriver::Graphics::ShaderPath::Vertex, "vertex routing changed");
                Require(state.stages.vertexWaveSize == (vertexWave32 ? 32u : 64u), "incorrect vertex wave size");
                Require(state.stages.fragmentWaveSize == (fragmentWave32 ? 32u : 64u), "incorrect fragment wave size");
            }
        }
    }
    queue = makeState();
    queue.context[0x2d5] = 0x2020;
    queue.userConfig[0x25b] = (64u << 9u) | 21u;
    queue.context[0x1ff] = 64;
    queue.context[0x2ce] = 3;
    queue.context[0x29b] = 2;
    queue.shader[0x8a] = 3u << 29u;
    queue.shader[0x8b] = 3u << 16u;
    auto stages = AgcDriver::Graphics::DecodeState(queue).stages;
    Require(stages.path == AgcDriver::Graphics::ShaderPath::Geometry && stages.mesh && stages.mesh->primitivesPerGroup == 21 && stages.mesh->verticesPerGroup == 63, "geometry assembly changed");
    queue.userConfig[0x25b] = 0;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "invalid geometry subgroup");
    queue = makeState();
    queue.context[0x2d5] = 0x200d;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "Patch topology and HS_EN disagree");
    queue.userConfig[0x242] = 9;
    queue.context[0x2d6] = (3u << 8u) | (3u << 14u);
    queue.context[0x2db] = 1u | (2u << 2u) | (2u << 5u);
    stages = AgcDriver::Graphics::DecodeState(queue).stages;
    Require(stages.path == AgcDriver::Graphics::ShaderPath::Tessellation && stages.tessellation && stages.tessellation->inputControlPoints == 3, "tessellation routing changed");
    queue.context[0x2d5] = 0x202d;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "combined tessellation and geometry");
    queue.context[0x2d5] = 0x200d;
    queue.context[0x2d6] = 0;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "control-point counts");
    queue = makeState();
    for (const auto value : {0x2003u, 0x2018u, 0x20c0u, 0x80002000u}) {
        queue.context[0x2d5] = value;
        expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "reserved");
    }
    for (const auto bit : {1u, 8u, 0x40u, 0x100u, 0x200u, 0x400u, 0x1000u, 0x4000u, 0x8000u, 0x80000u, 0x200000u, 0x800000u, 0x1000000u}) {
        queue.context[0x2d5] = 0x2000u | bit;
        expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "unsupported vertex");
    }
    queue.context[0x2d5] = 0;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "without PRIMGEN_EN");
    queue.context.erase(0x2d5);
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "missing register");
    queue.context[0x2d5] = 0x2000;
    queue.context.erase(0x1b6);
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "missing register");
}

void InitialContextTests() {
    const auto configured = makeState();
    AgcDriver::QueueState queue;
    Require(queue.context.at(0x3) == 0 && queue.context.at(0x8) == 0 && queue.context.at(0x9) == 0x3f800000, "depth bounds overlap render override");
    Require(queue.context.at(0x2dc) == 0xaa00 && queue.context.at(0x313) == 0x6000 && queue.context.at(0x2f9) == 0x2d, "initial raster controls are incomplete");
    Require(queue.context.at(0x30e) == 0xffffffff && queue.context.at(0x30f) == 0xffffffff, "initial sample mask excludes samples");
    queue.userConfig[0x242] = 4;
    for (const auto offset : {0x2d5u, 0x204u, 0x8eu, 0x8fu, 0x1c3u, 0x1c5u, 0x31cu, 0x3b0u, 0x3b8u, 0x318u, 0x390u, 0x10fu, 0x110u, 0x111u, 0x112u, 0x113u, 0x114u, 0xb4u, 0xb5u}) queue.context.at(offset) = configured.context.at(offset);
    const auto state = AgcDriver::Graphics::DecodeState(queue);
    Require(state.color.address == reinterpret_cast<std::uintptr_t>(colorMemory.data()) && state.color.bytes == colorMemory.size(), "sparse guest setup lost its render target");
    Require(queue.context.at(0x206) == 0x43f, "initial homogeneous viewport mode changed");
    for (const auto control : {0x3fu, 0x43eu, 0x53fu, 0x63fu, 0x8000043fu}) {
        queue.context[0x206] = control;
        expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "PA_CL_VTE_CNTL=0x");
    }
    queue.context[0x206] = 0x43f;
    queue.context[0x2dc] |= 1;
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "alpha-to-coverage");
    queue.context.erase(0x2dc);
    expectFailure([&] { AgcDriver::Graphics::DecodeState(queue); }, "missing register");
    queue.ClearContext();
    Require(queue.context.at(0x2dc) == 0xaa00 && queue.context.at(0x318) == 0 && queue.context.at(0x8e) == 0, "clear did not restore controls and discard target state");
    Require(!queue.context.contains(0xdead), "unknown context register acquired a default");
}

void resourceTests() {
    AgcDriver::Graphics::Context context{};
    context.limits.maxBoundDescriptorSets = 2;
    context.limits.maxStorageBufferRange = 4096;
    context.limits.maxUniformBufferRange = 4096;
    const auto state = AgcDriver::Graphics::DecodeState(makeState());
    ShaderRecompiler::RecompileResult vertex;
    ShaderRecompiler::RecompileResult fragment;
    vertex.bindings.push_back({ShaderRecompiler::DescriptorKind::SampledImage, 0, 0, 1, {}});
    expectFailure([&] { AgcDriver::Graphics::ShaderResources resources(context, vertex, fragment, state.color, 0, 0); }, "image, sampler");
    auto& binding = vertex.bindings.front();
    binding.kind = ShaderRecompiler::DescriptorKind::StorageBuffer;
    const auto address = state.color.address;
    binding.guestDescriptor = {static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(address >> 32u), 64, 0x31000000};
    expectFailure([&] { AgcDriver::Graphics::ShaderResources resources(context, vertex, fragment, state.color, 0, 0); }, "aliases the render target");
    binding.guestDescriptor[1] |= 16u << 16u;
    expectFailure([&] { AgcDriver::Graphics::ShaderResources resources(context, vertex, fragment, state.color, 0, 0); }, "strided");
    binding.count = 2;
    expectFailure([&] { AgcDriver::Graphics::ShaderResources resources(context, vertex, fragment, state.color, 0, 0); }, "descriptor array");
}

}

int main() {
    try {
        stateTests();
        ShaderStageTests();
        InitialContextTests();
        resourceTests();
        std::cout << "Graphics validation tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
