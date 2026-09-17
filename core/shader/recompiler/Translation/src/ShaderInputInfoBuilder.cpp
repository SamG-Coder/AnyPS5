#include "Translation/ShaderInputInfoBuilder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

namespace {

thread_local ShaderPixelInputInfo pixelStorage;
thread_local ShaderComputeInputInfo computeStorage;

}

ShaderStageInputInfo BuildShaderStageInputInfo(ShaderStageKind stage, const GuestContext& context) {
    switch (stage) {
    case ShaderStageKind::Compute: {
        if (!context.compute.has_value()) {
            throw std::runtime_error("ShaderInputInfoBuilder: GuestContext.compute is not set");
        }
        const auto& compute = *context.compute;
        computeStorage = ShaderComputeInputInfo{};
        computeStorage.threadsNum[0] = compute.numThreads[0];
        computeStorage.threadsNum[1] = compute.numThreads[1];
        computeStorage.threadsNum[2] = compute.numThreads[2];
        computeStorage.ldsSizeDwords = compute.ldsSizeDwords;
        computeStorage.waveSize = context.waveSize;
        computeStorage.groupId[0] = compute.groupIdEnable[0];
        computeStorage.groupId[1] = compute.groupIdEnable[1];
        computeStorage.groupId[2] = compute.groupIdEnable[2];
        computeStorage.tgSizeEn = compute.tgSizeEnable;
        computeStorage.threadIdsNum = static_cast<int>(compute.threadIdComponentCount);
        ShaderStageInputInfo result;
        result.compute = &computeStorage;
        return result;
    }
    case ShaderStageKind::Pixel: {
        if (!context.pixel.has_value()) {
            throw std::runtime_error("ShaderInputInfoBuilder: GuestContext.pixel is not set");
        }
        const auto& pixel = *context.pixel;
        pixelStorage = ShaderPixelInputInfo{};
        for (std::uint32_t i = 0; i < 32; ++i) {
            pixelStorage.interpolatorSettings[i] = pixel.interpolatorSettings[i];
        }
        pixelStorage.inputNum = pixel.interpolatorCount;
        if (pixel.hasPerspectiveCenterVgpr) {
            pixelStorage.psPerspectiveCenterVgpr = pixel.perspectiveCenterVgpr;
        }
        for (std::uint32_t i = 0; i < 8; ++i) {
            pixelStorage.targetOutputMode[i] = pixel.targetOutputMode[i];
        }
        throw std::runtime_error("ShaderInputInfoBuilder: color component mapping source is not available");
    }
    case ShaderStageKind::Vertex:
    case ShaderStageKind::Local:
    case ShaderStageKind::TessellationControl:
    case ShaderStageKind::TessellationEvaluation:
    case ShaderStageKind::Mesh:
        throw std::runtime_error("ShaderInputInfoBuilder: vertex/tessellation/mesh input info is not implemented, embedded fetch metadata source is unavailable");
    case ShaderStageKind::Unknown:
    case ShaderStageKind::Fetch:
        throw std::runtime_error("ShaderInputInfoBuilder: unexpected stage");
    }
    throw std::runtime_error("ShaderInputInfoBuilder: unexpected stage");
}

}
