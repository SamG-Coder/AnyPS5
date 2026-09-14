#ifndef CORE_SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_INCLUDE_INTERMEDIATEREPRESENTATION_IRMETADATA_HPP
#define CORE_SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_INCLUDE_INTERMEDIATEREPRESENTATION_IRMETADATA_HPP

#include "IntermediateRepresentation/IrValue.hpp"
#include "ControlFlow/ControlFlowGraph.hpp"
#include "RdnaDecoder/RdnaInstruction.hpp"
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace ShaderRecompiler {

enum class IrShaderStage {
    Unknown,
    Vertex,
    Pixel,
    Fetch,
    Compute,
    Mesh,
    Local,
    TessellationControl,
    TessellationEvaluation
};

enum class IrBufferFormat : std::uint32_t {
    Invalid = 0,
    Format8UNorm = 1,
    Format8SNorm = 2,
    Format8UScaled = 3,
    Format8SScaled = 4,
    Format8UInt = 5,
    Format8SInt = 6,
    Format16UNorm = 7,
    Format16SNorm = 8,
    Format16UScaled = 9,
    Format16SScaled = 10,
    Format16UInt = 11,
    Format16SInt = 12,
    Format16Float = 13,
    Format8_8UNorm = 14,
    Format8_8SNorm = 15,
    Format8_8UScaled = 16,
    Format8_8SScaled = 17,
    Format8_8UInt = 18,
    Format8_8SInt = 19,
    Format32UInt = 20,
    Format32SInt = 21,
    Format32Float = 22,
    Format16_16UNorm = 23,
    Format16_16SNorm = 24,
    Format16_16UScaled = 25,
    Format16_16SScaled = 26,
    Format16_16UInt = 27,
    Format16_16SInt = 28,
    Format16_16Float = 29,
    Format11_11_10UNorm = 30,
    Format11_11_10SNorm = 31,
    Format11_11_10UScaled = 32,
    Format11_11_10SScaled = 33,
    Format11_11_10UInt = 34,
    Format11_11_10SInt = 35,
    Format11_11_10Float = 36,
    Format10_11_11UNorm = 37,
    Format10_11_11SNorm = 38,
    Format10_11_11UScaled = 39,
    Format10_11_11SScaled = 40,
    Format10_11_11UInt = 41,
    Format10_11_11SInt = 42,
    Format10_11_11Float = 43,
    Format2_10_10_10UNorm = 44,
    Format2_10_10_10SNorm = 45,
    Format2_10_10_10UScaled = 46,
    Format2_10_10_10SScaled = 47,
    Format2_10_10_10UInt = 48,
    Format2_10_10_10SInt = 49,
    Format10_10_10_2UNorm = 50,
    Format10_10_10_2SNorm = 51,
    Format10_10_10_2UScaled = 52,
    Format10_10_10_2SScaled = 53,
    Format10_10_10_2UInt = 54,
    Format10_10_10_2SInt = 55,
    Format8_8_8_8UNorm = 56,
    Format8_8_8_8SNorm = 57,
    Format8_8_8_8UScaled = 58,
    Format8_8_8_8SScaled = 59,
    Format8_8_8_8UInt = 60,
    Format8_8_8_8SInt = 61,
    Format32_32UInt = 62,
    Format32_32SInt = 63,
    Format32_32Float = 64,
    Format16_16_16_16UNorm = 65,
    Format16_16_16_16SNorm = 66,
    Format16_16_16_16UScaled = 67,
    Format16_16_16_16SScaled = 68,
    Format16_16_16_16UInt = 69,
    Format16_16_16_16SInt = 70,
    Format16_16_16_16Float = 71,
    Format32_32_32UInt = 72,
    Format32_32_32SInt = 73,
    Format32_32_32Float = 74,
    Format32_32_32_32UInt = 75,
    Format32_32_32_32SInt = 76,
    Format32_32_32_32Float = 77,
    Format8Srgb = 128,
    Format8_8Srgb = 129,
    Format8_8_8_8Srgb = 130,
    Format10_10_10_2Float = 131,
    Format9_9_9_5Float = 132,
    Format5_6_5UNorm = 133,
    Format5_5_5_1UNorm = 134,
    Format1_5_5_5UNorm = 135,
    Format4_4_4_4UNorm = 136,
    Fmask8_S2_F1 = 156,
    Fmask8_S4_F1 = 157,
    Fmask8_S8_F1 = 158,
    Fmask8_S2_F2 = 159,
    Fmask8_S4_F2 = 160,
    Fmask8_S4_F4 = 161,
    Fmask16_S16_F1 = 162,
    Fmask16_S8_F2 = 163,
    Fmask32_S16_F2 = 164,
    Fmask32_S8_F4 = 165,
    Fmask32_S8_F8 = 166,
    Fmask64_S16_F4 = 167,
    Fmask64_S16_F8 = 168,
    Bc1UNorm = 169,
    Bc1Srgb = 170,
    Bc2UNorm = 171,
    Bc2Srgb = 172,
    Bc3UNorm = 173,
    Bc3Srgb = 174,
    Bc4UNorm = 175,
    Bc4SNorm = 176,
    Bc5UNorm = 177,
    Bc5SNorm = 178,
    Bc6UFloat = 179,
    Bc6SFloat = 180,
    Bc7UNorm = 181,
    Bc7Srgb = 182,
};

enum class IrTextureNumericClass { Unsupported, Float, Uint, Sint };

inline constexpr std::uint32_t ShaderImageIdentitySwizzle = 0x00000facu;

enum class ResourceKind {
    None,
    ScalarBuffer,
    ScalarAddress,
    Buffer,
    Flat,
    Global,
    Scratch,
    Lds,
    Gds,
    Image,
    Sampler
};

struct MemoryInfo {
    ResourceKind kind = ResourceKind::None;
    std::uint32_t resource = 0;
    std::uint32_t sampler = 0;
    std::uint32_t offset = 0;
    std::uint32_t secondaryOffset = 0;
    std::uint32_t dmask = 0;
    std::uint32_t dataDwords = 1;
    std::uint32_t dataBits = 32;
    std::uint32_t componentIndex = 0;
    std::uint32_t componentCount = 1;
    std::uint32_t dataFormat = 0;
    std::uint32_t numberFormat = 0;
    std::uint32_t imageSampleFlags = 0;
    RdnaImageDimension imageDimension = RdnaImageDimension::Unknown;
    std::uint32_t imageAddressComponents = 0;
    bool addressIsFull = false;
    bool dataSigned = false;
    bool typed = false;
    bool formatted = false;
    bool imageHasMip = false;
    bool imageR128 = false;
    bool idxen = false;
    bool offen = false;
    bool planningOnly = false;

    bool operator==(const MemoryInfo& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

enum class ExportTargetKind { Unknown, Null, Position, Primitive, Parameter, Mrt, MrtZ };

struct ExportInfo {
    ExportTargetKind kind = ExportTargetKind::Unknown;
    std::uint32_t target = 0;
    std::uint32_t index = 0;
    std::uint32_t en = 0;
    bool done = false;
    bool compr = false;
    bool vm = false;

    bool operator==(const ExportInfo& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct BufferResource {
    static constexpr std::uint32_t NoImageAlias = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t source = 0;
    std::uint32_t firstUsePc = 0;
    std::uint32_t maxByteExtent = 0;
    std::uint32_t packedStride = 0;
    IrBufferFormat descriptorFormat = IrBufferFormat::Invalid;
    std::uint32_t descriptorSwizzle = 0x00000facu;
    std::uint32_t imageAlias = NoImageAlias;
    bool read = false;
    bool written = false;
    bool atomic = false;
    bool formatted = false;
    bool scalar = false;

    bool operator==(const BufferResource& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

enum class ImageMipMode { None, DynamicStorage };

struct ImageResource {
    static constexpr std::uint32_t NoIndirectImage = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t source = 0;
    std::uint32_t firstUsePc = 0;
    ImageResourceClass resourceClass = ImageResourceClass::None;
    IrTextureNumericClass numericClass = IrTextureNumericClass::Unsupported;
    RdnaImageDimension dimension = RdnaImageDimension::Unknown;
    ImageMipMode mipMode = ImageMipMode::None;
    std::uint32_t mipCount = 1;
    IrBufferFormat conversionFormat = IrBufferFormat::Invalid;
    std::uint32_t shaderSwizzle = ShaderImageIdentitySwizzle;
    bool read = false;
    bool written = false;
    bool atomic = false;
    bool depthCompare = false;
    bool cube = false;
    bool r128 = false;
    std::uint32_t indirectRoot = NoIndirectImage;
    std::uint32_t indirectMappingOffset = 0;
    std::uint32_t indirectSearchIterations = 0;
    std::vector<std::uint32_t> indirectResources;

    bool operator==(const ImageResource& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct SamplerResource {
    std::uint32_t source = 0;
    std::uint32_t firstUsePc = 0;
    bool forcePointFiltering = false;
    bool depthCompare = false;

    bool operator==(const SamplerResource& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct SampledResourcePair {
    std::uint32_t image = 0;
    std::uint32_t sampler = 0;
    std::uint32_t firstUsePc = 0;

    bool operator==(const SampledResourcePair& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

enum class TessellationAttribute {
    LocalOutput,
    ControlInput,
    ControlOutput,
    EvaluationInput,
    PatchOutput,
    Factor
};

enum class StageInputKind {
    VertexIndex,
    InvocationId,
    PrimitiveId,
    TessCoord,
    InstanceIndex,
    FragCoord,
    FrontFacing,
    PackedAncillary,
    Layer,
    SampleId,
    BaryCoordSmooth,
    BaryCoordNoPerspective,
    WorkgroupId,
    LocalInvocationId,
    LocalInvocationIndex,
    GlobalInvocationId,
    Parameter,
};

enum class StageOutputKind {
    Position,
    Parameter,
    Mrt,
    Depth,
    SampleMask,
    PointSize,
    ClipDistance,
    CullDistance,
    Layer,
    ViewportIndex
};

struct PositionExportComponent {
    std::uint32_t clipDistance = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t cullDistance = std::numeric_limits<std::uint32_t>::max();
    bool pointSize = false;
    bool layer = false;
    bool viewport = false;
};

struct StageInput {
    StageInputKind kind = StageInputKind::VertexIndex;
    std::uint32_t location = 0;
    std::uint32_t componentCount = 1;
    std::string debugName;
    bool perVertex = false;

    bool operator==(const StageInput& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct StageOutput {
    StageOutputKind kind = StageOutputKind::Parameter;
    std::uint32_t index = 0;
    std::uint32_t location = 0;
    std::string debugName;

    bool operator==(const StageOutput& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

inline constexpr std::uint32_t FirstImageBinding = 1u;
inline constexpr std::uint32_t FirstComparisonImageBinding = 22u;
inline constexpr std::uint32_t FirstStorageImageBinding = 29u;
inline constexpr std::uint32_t ImageBindingCount = 43u;

enum class DescriptorBindingKind : std::uint32_t {
    Buffers = 0u,
    Samplers = FirstImageBinding + ImageBindingCount,
    Gds,
    BdaPagetable,
    FaultBuffer,
    FlattenedSrt,
    ShaderData,
    Count,
};

struct PushData {
    static constexpr std::uint32_t DwordCount = 32;
    static constexpr std::uint32_t MeshDrawDwordCount = 6;
    static constexpr std::uint32_t NoStart = std::numeric_limits<std::uint32_t>::max();
    std::array<std::uint32_t, DwordCount> dwords {};

    [[nodiscard]] static bool CanFit(std::uint32_t start, std::uint32_t size) {
        throw std::runtime_error("CanFit not implemented");
    }
    [[nodiscard]] static std::uint32_t StartFor(std::uint32_t cursor, std::uint32_t size) {
        throw std::runtime_error("StartFor not implemented");
    }
};

struct IrDescriptorBinding {
    DescriptorBindingKind kind = DescriptorBindingKind::Buffers;
    std::vector<std::uint32_t> resources;

    bool operator==(const IrDescriptorBinding& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct IrBindingLayout {
    std::uint32_t pushDataStartDword = PushData::NoStart;
    std::uint32_t memoryOffsetDword = 0;
    std::uint32_t memoryOffsetCount = 0;
    std::vector<std::uint32_t> userDataRegisters;
    std::vector<IrDescriptorBinding> descriptors;

    [[nodiscard]] std::uint32_t ShaderDataDwords() const {
        throw std::runtime_error("ShaderDataDwords not implemented");
    }
    [[nodiscard]] bool UsesPushData() const {
        throw std::runtime_error("UsesPushData not implemented");
    }
    void AdvancePushData(std::uint32_t& cursor) const {
        throw std::runtime_error("AdvancePushData not implemented");
    }

    bool operator==(const IrBindingLayout& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct ShaderInfo {
    std::uint32_t scratchDwords = 0;
    std::uint32_t sharedMemoryBytes = 0;
    static constexpr std::uint32_t MaxBuffers = 32;
    static constexpr std::uint32_t MaxImages = 64;
    static constexpr std::uint32_t MaxSamplers = 32;
    static constexpr std::uint32_t MaxSampledPairs = 64;

    std::vector<BufferResource> buffers;
    std::vector<ImageResource> images;
    std::vector<SamplerResource> samplers;
    std::vector<SampledResourcePair> sampledPairs;
    std::vector<StageInput> inputs;
    std::vector<StageOutput> outputs;
    std::array<std::uint8_t, 32> vertexFetchComponents {};
    std::int32_t vertexOffsetSgpr = -1;
    std::int32_t instanceOffsetSgpr = -1;
    bool hasBitwiseXor = false;
    bool usesDma = false;

    bool operator==(const ShaderInfo& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct BlockInfo {
    std::uint32_t id = 0;
    std::uint32_t startPc = 0;
    std::uint32_t endPc = 0;
    Terminator terminator;
    IrValue* condition = nullptr;
    IrValue* indirectTarget = nullptr;
};

struct DescriptorSource {
    struct IndirectImage {
        std::uint32_t materialSource = 0;
        std::uint32_t heapSource = 0;
        std::uint32_t selectorStride = 0;
        std::uint32_t selectorOffset = 0;
        std::uint32_t keyArg = 0;

        bool operator==(const IndirectImage& other) const {
            throw std::runtime_error("operator== not implemented");
        }
    };

    std::array<IrValue*, 8> dwords {};
    std::uint32_t dwordCount = 0;
    std::optional<IndirectImage> indirectImage;

    bool operator==(const DescriptorSource& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct SrtRead {
    IrValue* value = nullptr;
    std::uint32_t flatOffset = 0;

    bool operator==(const SrtRead& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct ResourceBlock {
    IrValue* condition = nullptr;
    std::vector<std::uint32_t> successors;
    std::vector<std::uint32_t> sources;
};

struct CompiledShaderInfo {
    IrShaderStage stage = IrShaderStage::Unknown;
    std::uint64_t shaderHash = 0;
    std::uint32_t waveSize = 64;
    std::uint32_t userDataBase = 0;
    std::uint32_t userDataCount = 64;
    std::uint32_t scratchDwords = 0;
    std::uint32_t paramExportMask = 0;
    ShaderInfo info;
    IrBindingLayout bindings;
};

struct DescriptorValue {
    std::array<std::uint32_t, 8> dwords = {};
    std::uint32_t dwordCount = 0;

    bool operator==(const DescriptorValue& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

enum class UniformFillKind { None, Buffer, Image };

struct UniformFill {
    UniformFillKind kind = UniformFillKind::None;
    std::uint32_t resource = 0;
    std::array<std::uint32_t, 3> groupStride {};
    std::uint32_t words = 0;
    std::uint32_t value = 0;

    bool operator==(const UniformFill& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct ResourceSnapshot {
    std::vector<DescriptorValue> buffers;
    std::vector<DescriptorValue> images;
    std::vector<DescriptorValue> samplers;
    std::vector<std::uint32_t> flattenedSrt;
    std::vector<std::uint32_t> userData;
    UniformFill uniformFill;
};

struct UniformFillPlan {
    UniformFill fill;
    std::array<IrValue*, 4> values{};
};

inline constexpr std::uint32_t NativePushConstantSize = sizeof(PushData);

struct IrResourcePlan {
    IrShaderStage stage = IrShaderStage::Unknown;
    std::uint64_t shaderHash = 0;
    std::uint32_t userDataBase = 0;
    std::uint32_t userDataCount = 64;
    std::vector<std::unique_ptr<IrValue>> valueStorage;
    std::vector<MemoryInfo> memoryInfo;
    std::vector<DescriptorSource> descriptorSources;
    std::vector<ResourceBlock> controlFlow;
    std::vector<std::uint32_t> materializationSources;
    std::vector<SrtRead> srtReads;
    std::vector<std::uint8_t> cleanFlatSlots;
    bool requiresSpecializationMemory = false;
    bool srtPlanComplete = false;
    bool resourceTrackingComplete = false;
    ShaderInfo info;
    UniformFillPlan uniformFill;
};

struct IrProgramMetadata {
    FailureKind cfgFailureKind = FailureKind::None;
    std::string failureReason;
    std::vector<BlockInfo> blockInfo;
    std::vector<ExportInfo> exportInfo;
    std::vector<IrValue*> dynamicReads;
    bool shaderInfoComplete = false;
    IrBindingLayout bindings;
    bool bindingLayoutComplete = false;
};

}

#endif
