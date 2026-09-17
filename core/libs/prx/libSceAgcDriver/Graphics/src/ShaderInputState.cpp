#include "prx/libSceAgcDriver/Graphics/include/ShaderInputState.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {
namespace {

constexpr std::uint32_t computeNumThreadX = 0x207;
constexpr std::uint32_t computeNumThreadY = 0x208;
constexpr std::uint32_t computeNumThreadZ = 0x209;
constexpr std::uint32_t computePgmRsrc2 = 0x213;

constexpr std::uint32_t spiPsInputCntl0 = 0x191;
constexpr std::uint32_t spiPsInputEna = 0x1B3;
constexpr std::uint32_t spiPsInputAddr = 0x1B4;
constexpr std::uint32_t spiPsInControl = 0x1B6;
constexpr std::uint32_t dbShaderControl = 0x203;
constexpr std::uint32_t spiShaderColFormat = 0x1C5;

std::uint32_t read(const Registers& registers, std::uint32_t offset) {
    const auto it = registers.find(offset);
    if (it == registers.end()) {
        throw std::runtime_error("AGC graphics: missing register at DWORD 0x" + std::to_string(offset));
    }
    return it->second;
}

}

ShaderRecompiler::ShaderComputeStageInfo DecodeComputeStageInfo(const Registers& shader) {
    const auto numThreadX = read(shader, computeNumThreadX);
    const auto numThreadY = read(shader, computeNumThreadY);
    const auto numThreadZ = read(shader, computeNumThreadZ);
    if (numThreadX == 0 || numThreadY == 0 || numThreadZ == 0) {
        throw std::runtime_error("AGC graphics: COMPUTE_NUM_THREAD_X/Y/Z must be nonzero");
    }
    const auto rsrc2 = read(shader, computePgmRsrc2);
    if ((rsrc2 & 0x1u) != 0) {
        throw std::runtime_error("AGC graphics: COMPUTE_PGM_RSRC2.SCRATCH_EN is unsupported");
    }
    return ShaderRecompiler::ShaderComputeStageInfo{
        {numThreadX, numThreadY, numThreadZ},
        ((rsrc2 >> 15u) & 0x1FFu) * 128u,
        {((rsrc2 >> 7u) & 0x1u) != 0, ((rsrc2 >> 8u) & 0x1u) != 0, ((rsrc2 >> 9u) & 0x1u) != 0},
        ((rsrc2 >> 10u) & 0x1u) != 0,
        ((rsrc2 >> 11u) & 0x3u) + 1u
    };
}

ShaderRecompiler::ShaderPixelStageInfo DecodePixelStageInfo(const Registers& context) {
    const auto inControl = read(context, spiPsInControl);
    const auto inputNum = inControl & 0x3Fu;
    if (inputNum > 32u) {
        throw std::runtime_error("AGC graphics: SPI_PS_IN_CONTROL input count exceeds 32");
    }
    const auto ena = read(context, spiPsInputEna);
    const auto addr = read(context, spiPsInputAddr);
    const auto activeInputs = ena & addr;
    constexpr std::uint32_t knownMask = 0x1u | 0x2u | 0x10u | 0x20u | 0x100u | 0x200u | 0x400u | 0x800u | 0x1000u | 0x2000u;
    if ((activeInputs & ~knownMask) != 0) {
        throw std::runtime_error("AGC graphics: unsupported SPI_PS_INPUT_ENA/ADDR bit combination");
    }
    std::array<std::uint32_t, 32> interpolatorSettings{};
    for (std::uint32_t i = 0; i < inputNum; ++i) {
        interpolatorSettings[i] = read(context, spiPsInputCntl0 + i);
    }
    const auto shaderControl = read(context, dbShaderControl);
    if (((shaderControl >> 9u) & 0x1u) != 0) {
        throw std::runtime_error("AGC graphics: DB_SHADER_CONTROL.DUAL_EXPORT_ENABLE is unsupported");
    }
    if (((shaderControl >> 11u) & 0x1u) != 0) {
        throw std::runtime_error("AGC graphics: DB_SHADER_CONTROL.ALPHA_TO_MASK_DISABLE is unsupported");
    }
    if (((shaderControl >> 13u) & 0x3u) != 0) {
        throw std::runtime_error("AGC graphics: DB_SHADER_CONTROL.CONSERVATIVE_Z_EXPORT is unsupported");
    }
    const auto colFormat = read(context, spiShaderColFormat);
    std::array<std::uint8_t, 8> targetOutputMode{};
    for (std::uint32_t i = 0; i < 8u; ++i) {
        targetOutputMode[i] = static_cast<std::uint8_t>((colFormat >> (4u * i)) & 0xFu);
    }
    const bool hasPerspectiveCenterVgpr = (activeInputs & 0x2u) != 0;
    const bool pixelKillEnable = ((shaderControl >> 6u) & 0x1u) != 0;
    const bool depthExportEnable = (shaderControl & 0x1u) != 0;
    const bool sampleMaskExportEnable = ((shaderControl >> 8u) & 0x1u) != 0;
    const auto zOrder = (shaderControl >> 4u) & 0x3u;
    return ShaderRecompiler::ShaderPixelStageInfo{
        inputNum,
        interpolatorSettings,
        (inControl & 0x8000u) != 0,
        hasPerspectiveCenterVgpr ? ((activeInputs & 0x1u) ? 2u : 0u) : 0u,
        hasPerspectiveCenterVgpr,
        (activeInputs & 0x100u) != 0,
        (activeInputs & 0x200u) != 0,
        (activeInputs & 0x400u) != 0,
        (activeInputs & 0x800u) != 0,
        (activeInputs & 0x1000u) != 0,
        (activeInputs & 0x2000u) != 0,
        (activeInputs & 0x11u) == 0x11u,
        (activeInputs & 0x20u) != 0,
        pixelKillEnable,
        depthExportEnable,
        sampleMaskExportEnable,
        zOrder == 1u && !pixelKillEnable && !depthExportEnable && !sampleMaskExportEnable,
        ((shaderControl >> 10u) & 0x1u) != 0,
        targetOutputMode
    };
}

}
