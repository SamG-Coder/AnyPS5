#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include "prx/libSceAgcDriver/Submit/include/Acb.hpp"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
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
        const std::array<std::uint32_t, 3> arguments{1, 1, 1};
        if (argc == 2 && std::string(argv[1]) == "indirect") {
            const auto argumentAddress = reinterpret_cast<std::uintptr_t>(arguments.data());
            commands[7] = 0xc0021600;
            commands[8] = static_cast<std::uint32_t>(argumentAddress);
            commands[9] = static_cast<std::uint32_t>(argumentAddress >> 32u);
            commands[10] = 0x8041;
            packet.dw_num = 11;
        }
        sceAgcDriverSubmitAcb(0x20, &packet);
        try {
            AgcDriverWaitIdle_nid_postfix();
        } catch (const std::runtime_error& error) {
            if (std::string(error.what()) != "ShaderRecompiler::Recompile not implemented") {
                throw;
            }
            std::puts("Vulkan device initialized; real recompiler exception propagated from compute dispatch");
            try {
                LibcRunShutdown_nid_postfix();
                throw std::runtime_error("shutdown lost recompiler failure");
            } catch (const std::runtime_error& shutdown) {
                if (std::string(shutdown.what()) != "ShaderRecompiler::Recompile not implemented") throw;
            }
            return 0;
        }
        throw std::runtime_error("dispatch unexpectedly completed without a recompiler");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        try { LibcRunShutdown_nid_postfix(); }
        catch (const std::exception& shutdown) { std::fprintf(stderr, "shutdown: %s\n", shutdown.what()); }
        return 1;
    }
}
