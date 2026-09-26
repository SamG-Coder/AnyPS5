#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libc/include/GuestMemoryBacking.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <memory>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {
using AgcDriver::Graphics::Require;
constexpr std::uint32_t extent = 64;
alignas(256) std::array<std::byte, extent * extent * 4> pixels;

ShaderRecompiler::RecompileResult loadShader(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    Require(file.is_open(), "cannot open completion test shader");
    const auto size = file.tellg();
    Require(size >= 20 && size <= 1024 * 1024 && size % 4 == 0, "invalid completion test shader");
    ShaderRecompiler::RecompileResult result;
    result.spirv.resize(static_cast<std::size_t>(size) / 4);
    file.seekg(0);
    Require(static_cast<bool>(file.read(reinterpret_cast<char*>(result.spirv.data()), size)), "cannot read completion test shader");
    return result;
}
}

int main(int argc, char** argv) {
    try {
        Require(argc == 3, "expected vertex and fragment SPIR-V paths");
        auto vertex = loadShader(argv[1]);
        auto fragment = loadShader(argv[2]);
        const std::array<AgcDriver::Graphics::CompiledShader, 2> shaders{{
            {ShaderRecompiler::ShaderStage::Vertex, &vertex, 0},
            {ShaderRecompiler::ShaderStage::Fragment, &fragment, 0}
        }};
        for (std::size_t i = 0; i < pixels.size(); i += 4) {
            pixels[i] = std::byte{16};
            pixels[i + 1] = std::byte{24};
            pixels[i + 2] = std::byte{40};
            pixels[i + 3] = std::byte{255};
        }
        const auto guest = std::shared_ptr<void>(
            GuestMemoryBacking::GuestMemoryBackingMap_nid_postfix(nullptr, pixels.size(), 65536, 3),
            [](void* memory) { GuestMemoryBacking::GuestMemoryBackingUnmap_nid_postfix(memory, pixels.size()); });
        Require(guest != nullptr, "cannot allocate completion render target");
        const auto address = reinterpret_cast<std::uintptr_t>(guest.get());
        AgcDriver::GuestMemory::Write(address, pixels, 256);
        AgcDriver::Graphics::State graphics{};
        graphics.stages.path = AgcDriver::Graphics::ShaderPath::Vertex;
        graphics.color = {address, {extent, extent},
            VK_FORMAT_R8G8B8A8_UNORM, pixels.size(), 0xe4};
        graphics.hasColorTarget = true;
        graphics.renderExtent = {extent, extent};
        graphics.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        graphics.viewport = {0, static_cast<float>(extent), static_cast<float>(extent), -static_cast<float>(extent), 0, 1};
        graphics.scissor = {{0, 0}, {extent, extent}};
        graphics.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        graphics.blend.colorWriteMask = 15;
        const AgcDriver::Graphics::DrawParameters draw{0, 3, 0, 1, 0, false};
        auto& runtime = AgcDriver::NativeGraphicsRuntime::Get();
        const auto cleanup = std::unique_ptr<AgcDriver::NativeGraphicsRuntime, void(*)(AgcDriver::NativeGraphicsRuntime*)>(
            &runtime, [](auto* value) { value->ReleaseWindow(nullptr); });
        runtime.Headless().EnqueueDraw(graphics, draw, shaders);
        std::array<std::uint32_t, 8> words{};
        CommandBuffer commands{words.data(), words.data() + words.size(), words.data(),
            words.data() + words.size(), nullptr, nullptr, 0};
        Label marker{17};
        aps5NativeAgcReleaseMem(&commands, 0x28, 0x30c, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
        Require(marker.value == 17, "completion published while recording");
        const Packet packet{words.data(), 8, 0, {0, 0, 0}};
        Require(aps5NativeAgcSubmit(&packet) == 0 && marker.value == 42, "completion was not published");
        AgcDriver::GuestMemory::Read(address, pixels, 256);
        const auto center = (extent / 2 * extent + extent / 2) * 4;
        Require(std::to_integer<unsigned>(pixels[center]) > 30 &&
            std::to_integer<unsigned>(pixels[center + 1]) > 30 &&
            std::to_integer<unsigned>(pixels[center + 2]) > 30,
            "completion visible before rendered pixels were available");
        Require(pixels[0] == std::byte{16} && pixels[1] == std::byte{24} && pixels[2] == std::byte{40},
            "completion draw changed background");
        std::cout << "Native completion: marker 17 -> 42; queued Vulkan triangle readback verified\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
