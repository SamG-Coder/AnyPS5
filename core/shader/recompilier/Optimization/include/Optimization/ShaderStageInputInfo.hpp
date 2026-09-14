#ifndef CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_SHADERSTAGEINPUTINFO_HPP
#define CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_SHADERSTAGEINPUTINFO_HPP

#include "IntermediateRepresentation/IrMetadata.hpp"
#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace ShaderRecompiler {

struct ShaderBufferResource {
    std::array<std::uint32_t, 4> fields{};
};

struct ShaderColorComponentMapping {
    static constexpr std::uint8_t Identity = 0xe4u;
    std::uint8_t packed = Identity;
};

struct ShaderVertexInputBuffer {
    static constexpr int MaxAttributes = 32;

    std::uint64_t addr = 0;
    std::uint32_t stride = 0;
    std::uint32_t numRecords = 0;
    std::uint32_t fetchIndex = 0;
    int attrNum = 0;
    int attrIndices[MaxAttributes] = {0};
    std::uint32_t attrOffsets[MaxAttributes] = {0};
};

struct ShaderVertexDestination {
    int registerStart = 0;
    int registersNum = 0;
    int attrId = -1;
    std::uint32_t fetchIndex = 0;
};

struct ShaderStageRuntime {
    const CompiledShaderInfo* program = nullptr;
    ResourceSnapshot resources;

    [[nodiscard]] explicit operator bool() const {
        throw std::runtime_error("shader input helper not implemented");
    }
};

struct ShaderClipSpaceTransform {
    float scale[2] = {};
    float offset[2] = {};
    float halfExtent[2] = {};
    bool enabled = false;
};

struct ShaderWorkgroupInputInfo {
    std::uint32_t threadsNum[3] = {0, 0, 0};
    std::uint32_t ldsSizeDwords = 0;
    std::uint32_t scratchSizeDwords = 0;
    std::uint32_t hostSubgroupSize = 64;
    std::uint32_t waveSize = 64;
};

struct ShaderMeshInputInfo: ShaderWorkgroupInputInfo {
    std::uint32_t inputPrimitive = 0;
    std::uint32_t primitivesPerGroup = 0;
    std::uint32_t verticesPerGroup = 0;
    std::uint32_t maxVertices = 0;
    std::uint32_t maxPrimitives = 0;
    std::uint32_t provokingVertex = 0;

    [[nodiscard]] std::uint32_t InputPrimitiveSize() const {
        throw std::runtime_error("shader input helper not implemented");
    }
    [[nodiscard]] std::uint32_t InputPrimitiveStep() const {
        throw std::runtime_error("shader input helper not implemented");
    }
    [[nodiscard]] std::uint32_t InputPrimitiveCount(std::uint32_t vertices) const {
        throw std::runtime_error("shader input helper not implemented");
    }
    [[nodiscard]] std::uint32_t InputVertexCount(std::uint32_t primitives) const {
        throw std::runtime_error("shader input helper not implemented");
    }
};

struct ShaderTessellationInputInfo {
    std::uint32_t inputControlPoints = 0;
    std::uint32_t outputControlPoints = 0;
    std::uint32_t lsStride = 0;
    std::uint32_t hsStride = 0;
    std::uint32_t domain = 0;
    std::uint32_t partitioning = 0;
    std::uint32_t outputTopology = 0;
};

struct ShaderVertexInputInfo {
    static constexpr int MaxResources = 32;

    ShaderBufferResource resources[MaxResources];
    ShaderVertexDestination resourcesDst[MaxResources];
    ShaderVertexInputBuffer buffers[MaxResources];
    ShaderStageRuntime stage;
    IrShaderStage logicalStage = IrShaderStage::Vertex;
    int resourcesNum = 0;
    int fetchAttribReg = 0;
    int fetchBufferReg = 0;
    int buffersNum = 0;
    std::uint32_t scratchSizeDwords = 0;
    std::uint32_t paClVsOutCntl = 0;
    ShaderClipSpaceTransform clipSpace;
    ShaderMeshInputInfo mesh;
    ShaderTessellationInputInfo tess;
    bool fetchExternal = false;
    bool fetchEmbedded = false;
};

struct ShaderComputeInputInfo: ShaderWorkgroupInputInfo {
    std::uint32_t dispatchThreadsNum[3] = {0, 0, 0};
    bool groupId[3] = {false, false, false};
    bool dispatchThreadDimensions = false;
    int threadIdsNum = 0;
    int workgroupRegister = 0;
    bool tgSizeEn = false;
    ShaderStageRuntime stage;
};

struct ShaderPixelInputInfo {
    std::uint32_t interpolatorSettings[32] = {0};
    std::uint32_t inputNum = 0;
    std::uint32_t psSystemInputBase = 0;
    std::uint32_t customInterpolationMask = 0;
    std::uint32_t psPerspectiveCenterVgpr = std::numeric_limits<std::uint32_t>::max();
    std::uint8_t targetOutputMode[8] = {};
    std::array<ShaderColorComponentMapping, 8> targetExportMapping = {};
    std::uint32_t scratchSizeDwords = 0;
    bool psPosX = false;
    bool psPosY = false;
    bool psPosZ = false;
    bool psPosW = false;
    bool psFrontFace = false;
    bool psAncillary = false;
    bool psNoPerspective = false;
    bool psPixelKillEnable = false;
    bool psDepthExportEnable = false;
    bool psSampleMaskExportEnable = false;
    bool psSampleShading = false;
    bool psEarlyZ = false;
    bool psExecuteOnNoop = false;
    ShaderStageRuntime stage;

    bool HasPositionInput() const {
        throw std::runtime_error("shader input helper not implemented");
    }
};

struct ShaderStageInputInfo {
    const ShaderVertexInputInfo* vertex = nullptr;
    const ShaderPixelInputInfo* pixel = nullptr;
    const ShaderComputeInputInfo* compute = nullptr;
};

}

#endif
