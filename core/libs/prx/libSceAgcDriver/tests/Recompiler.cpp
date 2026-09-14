#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Submit/include/Acb.hpp"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>

int main() {
    try {
        alignas(256) const std::array<std::uint32_t, 1> code{0xbf810000};
        Shader shader{};
        shader.file_header = 0x34333231;
        shader.version = 0x18;
        shader.header_size = sizeof(Shader);
        shader.shader_size = sizeof(code);
        shader.code = code.data();
        AgcDriverRegisterShader_nid_postfix(&shader);
        const auto address = reinterpret_cast<std::uintptr_t>(code.data());
        std::array<std::uint32_t, 12> commands{
            0xc0027600, 0x20c, static_cast<std::uint32_t>(address >> 8u), static_cast<std::uint32_t>(address >> 40u),
            0xc0017600, 0x213, 0,
            0xc0031500, 1, 1, 1, 0x8041
        };
        Packet packet{commands.data(), static_cast<std::uint32_t>(commands.size()), 0, {}};
        sceAgcDriverSubmitAcb(0x20, &packet);
        try {
            AgcDriverWaitIdle_nid_postfix();
        } catch (const std::runtime_error& error) {
            if (std::string(error.what()) != "ShaderRecompiler::Recompile not implemented") {
                throw;
            }
            std::puts("Vulkan device initialized; real recompiler exception propagated from compute dispatch");
            return 0;
        }
        throw std::runtime_error("dispatch unexpectedly completed without a recompiler");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
