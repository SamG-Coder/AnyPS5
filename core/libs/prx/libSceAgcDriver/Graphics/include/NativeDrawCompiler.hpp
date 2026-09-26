#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEDRAWCOMPILER_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEDRAWCOMPILER_HPP
#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DrawParameters.hpp"
#include "Recompiler.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>
namespace AgcDriver { class VulkanDevice; }
namespace AgcDriver::Graphics {
struct NativeShaderProgram {
    ShaderRecompiler::ShaderBinary binary;
    ShaderRecompiler::ProgramRole role;
    std::uint32_t userDataBase;
    std::uint32_t firstUserSgpr;
    std::vector<std::uint32_t> userData;
    std::uint64_t missingUserData = 0;
};
void CompileAndEnqueueNativeDraw(VulkanDevice& device, const State& graphics, DrawParameters draw,
    std::span<const NativeShaderProgram> programs,
    const ShaderRecompiler::ShaderPixelStageInfo& pixel,
    std::span<const ShaderRecompiler::MemoryRegion> initialMemory);
}
#endif
