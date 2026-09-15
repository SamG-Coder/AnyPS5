#include "SpirvBackend/SpirvModuleEmitter.hpp"
#include "SpirvBackend/SpirvEmitterHelpers.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void EmitModuleHeader(SpirvModule& module, const IrProgram& program, const BindingAllocationResult& bindings) {
    throw std::runtime_error("EmitModuleHeader not implemented");
}

void EmitModuleHeader(SpirvEmitterState& state, const BindingAllocationResult& bindings) {
    throw std::runtime_error("EmitModuleHeader not implemented");
}

const RdnaImageDimensionInfo& RdnaImageDimensionInfoFor(RdnaImageDimension dimension) {
    throw std::runtime_error("RdnaImageDimensionInfoFor not implemented");
}

std::uint32_t TypeVoid(SpirvEmitterState& state) {
    throw std::runtime_error("TypeVoid not implemented");
}

std::uint32_t TypeBool(SpirvEmitterState& state) {
    throw std::runtime_error("TypeBool not implemented");
}

std::uint32_t TypeBoolVector(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("TypeBoolVector not implemented");
}

std::uint32_t TypeU32(SpirvEmitterState& state) {
    throw std::runtime_error("TypeU32 not implemented");
}

std::uint32_t TypeU64(SpirvEmitterState& state) {
    throw std::runtime_error("TypeU64 not implemented");
}

std::uint32_t TypeScalarU64(SpirvEmitterState& state) {
    throw std::runtime_error("TypeScalarU64 not implemented");
}

std::uint32_t TypeU32Pair(SpirvEmitterState& state) {
    throw std::runtime_error("TypeU32Pair not implemented");
}

std::uint32_t TypeI32(SpirvEmitterState& state) {
    throw std::runtime_error("TypeI32 not implemented");
}

std::uint32_t TypeI32Pair(SpirvEmitterState& state) {
    throw std::runtime_error("TypeI32Pair not implemented");
}

std::uint32_t TypeF32(SpirvEmitterState& state) {
    throw std::runtime_error("TypeF32 not implemented");
}

std::uint32_t TypeU32Vector(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("TypeU32Vector not implemented");
}

std::uint32_t TypeU32Composite(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("TypeU32Composite not implemented");
}

std::uint32_t TypeI32Vector(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("TypeI32Vector not implemented");
}

std::uint32_t TypeF32Vector(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("TypeF32Vector not implemented");
}

std::uint32_t TypePointer(SpirvEmitterState& state, std::uint32_t storageClass, std::uint32_t pointee) {
    throw std::runtime_error("TypePointer not implemented");
}

std::uint32_t TypeFunction(SpirvEmitterState& state) {
    throw std::runtime_error("TypeFunction not implemented");
}

std::uint32_t TypeStorageBufferPointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypeStorageBufferPointer not implemented");
}

std::uint32_t TypeStorageBufferElementPointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypeStorageBufferElementPointer not implemented");
}

std::uint32_t TypeStorageBufferU64Pointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypeStorageBufferU64Pointer not implemented");
}

std::uint32_t TypeStorageBufferU64ElementPointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypeStorageBufferU64ElementPointer not implemented");
}

std::uint32_t TypePhysicalU32Pointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypePhysicalU32Pointer not implemented");
}

std::uint32_t TypePushConstantElementPointer(SpirvEmitterState& state) {
    throw std::runtime_error("TypePushConstantElementPointer not implemented");
}

std::uint32_t TypeU32ArrayPointer(SpirvEmitterState& state, std::uint32_t storageClass, std::uint32_t dwords) {
    throw std::runtime_error("TypeU32ArrayPointer not implemented");
}

std::uint32_t TypeU32ElementPointer(SpirvEmitterState& state, std::uint32_t storageClass) {
    throw std::runtime_error("TypeU32ElementPointer not implemented");
}

std::uint32_t TypeId(SpirvEmitterState& state, IrType type) {
    throw std::runtime_error("TypeId not implemented");
}

std::uint32_t GlslStd450(SpirvEmitterState& state) {
    throw std::runtime_error("GlslStd450 not implemented");
}

std::uint32_t PixelParameterLocation(const SpirvEmitterState& state, std::uint32_t attr) {
    throw std::runtime_error("PixelParameterLocation not implemented");
}

bool PixelParameterIsFlat(const SpirvEmitterState& state, std::uint32_t attr) {
    throw std::runtime_error("PixelParameterIsFlat not implemented");
}

bool PixelParameterIsCustom(const SpirvEmitterState& state, std::uint32_t attr) {
    throw std::runtime_error("PixelParameterIsCustom not implemented");
}

VertexInputScalarKind VertexParameterScalarKind(const SpirvEmitterState& state, std::uint32_t location) {
    throw std::runtime_error("VertexParameterScalarKind not implemented");
}

std::uint32_t VertexParameterComponentCount(const SpirvInputBinding& input) {
    throw std::runtime_error("VertexParameterComponentCount not implemented");
}

std::uint32_t VertexParameterScalarType(SpirvEmitterState& state, VertexInputScalarKind kind) {
    throw std::runtime_error("VertexParameterScalarType not implemented");
}

std::uint32_t OutputVariableForExport(const SpirvEmitterState& state, const ExportInfo& exp) {
    throw std::runtime_error("OutputVariableForExport not implemented");
}

std::uint32_t ConstantU32(SpirvEmitterState& state, std::uint32_t value) {
    throw std::runtime_error("ConstantU32 not implemented");
}

std::uint32_t EmitSubgroupLocalInvocationId(SpirvEmitterState& state) {
    throw std::runtime_error("EmitSubgroupLocalInvocationId not implemented");
}

[[noreturn]] void ExitDescriptorBindingFailure(const SpirvEmitterState& state, DescriptorBindingKind kind, std::uint32_t resource, const char* reason) {
    throw std::runtime_error("ExitDescriptorBindingFailure not implemented");
}

std::uint32_t ResourceForDescriptor(const SpirvEmitterState& state, DescriptorBindingKind kind, std::uint32_t resource) {
    throw std::runtime_error("ResourceForDescriptor not implemented");
}

std::uint32_t DescriptorElementPointer(SpirvEmitterState& state, std::uint32_t resultPtrType, std::uint32_t variableId, std::uint32_t arrayIndex, DescriptorBindingKind kind, std::uint32_t resource, const char* variableName) {
    throw std::runtime_error("DescriptorElementPointer not implemented");
}

std::uint32_t ImageScalarType(SpirvEmitterState& state, IrTextureNumericClass numericClass) {
    throw std::runtime_error("ImageScalarType not implemented");
}

std::uint32_t ImageVectorType(SpirvEmitterState& state, IrTextureNumericClass numericClass, std::uint32_t components) {
    throw std::runtime_error("ImageVectorType not implemented");
}

std::uint32_t ImageType(SpirvEmitterState& state, const ImageResource& image) {
    throw std::runtime_error("ImageType not implemented");
}

std::uint32_t ImageViewSizeType(SpirvEmitterState& state, RdnaImageDimension dimension) {
    throw std::runtime_error("ImageViewSizeType not implemented");
}

std::uint32_t LoadSampledImageDescriptor(SpirvEmitterState& state, std::uint32_t resource) {
    throw std::runtime_error("LoadSampledImageDescriptor not implemented");
}

std::uint32_t LoadSamplerDescriptor(SpirvEmitterState& state, std::uint32_t sampler) {
    throw std::runtime_error("LoadSamplerDescriptor not implemented");
}

std::uint32_t MakeSampledImage(SpirvEmitterState& state, std::uint32_t resource, std::uint32_t sampler) {
    throw std::runtime_error("MakeSampledImage not implemented");
}

std::uint32_t StorageImageDescriptorPointer(SpirvEmitterState& state, std::uint32_t resource) {
    throw std::runtime_error("StorageImageDescriptorPointer not implemented");
}

void EmitStorageImageWrite(SpirvEmitterState& state, std::uint32_t resource, std::uint32_t mipLod, std::uint32_t coord, std::uint32_t texel) {
    throw std::runtime_error("EmitStorageImageWrite not implemented");
}

std::uint32_t ExecutionModelForStage(IrShaderStage stage) {
    throw std::runtime_error("ExecutionModelForStage not implemented");
}

std::uint32_t ConstantI32(SpirvEmitterState& state, std::int32_t value) {
    throw std::runtime_error("ConstantI32 not implemented");
}

std::uint32_t ConstantF32(SpirvEmitterState& state, std::uint32_t bits) {
    throw std::runtime_error("ConstantF32 not implemented");
}

std::uint32_t FloatBits(float value) {
    throw std::runtime_error("FloatBits not implemented");
}

std::uint32_t ConstantF32Value(SpirvEmitterState& state, float value) {
    throw std::runtime_error("ConstantF32Value not implemented");
}

std::uint32_t ConstantBool(SpirvEmitterState& state, bool value) {
    throw std::runtime_error("ConstantBool not implemented");
}

std::uint32_t ConstantU64(SpirvEmitterState& state, std::uint64_t value) {
    throw std::runtime_error("ConstantU64 not implemented");
}

std::uint32_t ConstantU32CompositeZero(SpirvEmitterState& state, std::uint32_t components) {
    throw std::runtime_error("ConstantU32CompositeZero not implemented");
}

std::uint32_t DefineInterfaceVariable(SpirvEmitterState& state, std::uint32_t type, std::uint32_t storage, const char* name) {
    throw std::runtime_error("DefineInterfaceVariable not implemented");
}

void DefineModule(SpirvEmitterState& state) {
    throw std::runtime_error("DefineModule not implemented");
}

void DefineTessellationInterfaces(SpirvEmitterState& state) {
    throw std::runtime_error("DefineTessellationInterfaces not implemented");
}

void DefineTessellationExecutionModes(SpirvEmitterState& state) {
    throw std::runtime_error("DefineTessellationExecutionModes not implemented");
}

void DefineMeshOutputs(SpirvEmitterState& state) {
    throw std::runtime_error("DefineMeshOutputs not implemented");
}

void EmitMeshEntryPoint(SpirvEmitterState& state) {
    throw std::runtime_error("EmitMeshEntryPoint not implemented");
}

void EmitMeshAllocate(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitMeshAllocate not implemented");
}

std::uint32_t MeshOutputPointer(SpirvEmitterState& state, StageOutputKind kind, std::uint32_t index) {
    throw std::runtime_error("MeshOutputPointer not implemented");
}

std::uint32_t MeshPrimitivePointer(SpirvEmitterState& state) {
    throw std::runtime_error("MeshPrimitivePointer not implemented");
}

DppTargetLane EmitDppQuadPermTargetLane(SpirvEmitterState& state, std::uint32_t subid, std::uint32_t control) {
    throw std::runtime_error("EmitDppQuadPermTargetLane not implemented");
}

DppTargetLane EmitDppRowShiftTargetLane(SpirvEmitterState& state, std::uint32_t subid, std::uint32_t amount, bool left) {
    throw std::runtime_error("EmitDppRowShiftTargetLane not implemented");
}

DppTargetLane EmitDppRowRotateRightTargetLane(SpirvEmitterState& state, std::uint32_t subid, std::uint32_t amount) {
    throw std::runtime_error("EmitDppRowRotateRightTargetLane not implemented");
}

DppTargetLane EmitDppMirrorTargetLane(SpirvEmitterState& state, std::uint32_t subid, bool halfRow) {
    throw std::runtime_error("EmitDppMirrorTargetLane not implemented");
}

DppTargetLane EmitDppTargetLane(SpirvEmitterState& state, std::uint32_t control) {
    throw std::runtime_error("EmitDppTargetLane not implemented");
}

std::uint32_t InputVariableForKind(const SpirvEmitterState& state, StageInputKind kind) {
    throw std::runtime_error("InputVariableForKind not implemented");
}

const SpirvInputBinding* SpirvInputBindingForParameter(const SpirvEmitterState& state, std::uint32_t location) {
    throw std::runtime_error("SpirvInputBindingForParameter not implemented");
}

std::uint32_t EmitVertexParameterComponentU32(SpirvEmitterState& state, const SpirvInputBinding& input, std::uint32_t component) {
    throw std::runtime_error("EmitVertexParameterComponentU32 not implemented");
}

std::uint32_t EmitInputComponentU32(SpirvEmitterState& state, StageInputKind kind, std::uint32_t component) {
    throw std::runtime_error("EmitInputComponentU32 not implemented");
}

std::uint32_t EmitLocalInvocationIndex(SpirvEmitterState& state) {
    throw std::runtime_error("EmitLocalInvocationIndex not implemented");
}

std::uint32_t EmitBallotLaneActiveBool(SpirvEmitterState& state, std::uint32_t ballot, std::uint32_t lane) {
    throw std::runtime_error("EmitBallotLaneActiveBool not implemented");
}

std::uint32_t EmitSubgroupLaneActiveBool(SpirvEmitterState& state, std::uint32_t lane) {
    throw std::runtime_error("EmitSubgroupLaneActiveBool not implemented");
}

std::uint32_t EmitBinaryU32(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t lhs, std::uint32_t rhs) {
    throw std::runtime_error("EmitBinaryU32 not implemented");
}

std::uint32_t EmitShaderDataDwordLoad(SpirvEmitterState& state, std::uint32_t dwordIndex) {
    throw std::runtime_error("EmitShaderDataDwordLoad not implemented");
}

std::uint32_t StorageBufferPackedStride(const SpirvEmitterState& state, const MemoryInfo& mem) {
    throw std::runtime_error("StorageBufferPackedStride not implemented");
}

IrBufferFormat StorageBufferFormat(const SpirvEmitterState& state, const MemoryInfo& mem) {
    throw std::runtime_error("StorageBufferFormat not implemented");
}

void EmitMemoryOffsets(SpirvEmitterState& state) {
    throw std::runtime_error("EmitMemoryOffsets not implemented");
}

std::uint32_t LdsDwordCount(const SpirvEmitterState& state) {
    throw std::runtime_error("LdsDwordCount not implemented");
}

MemoryResourceAccess PrepareMemoryResourceAccess(SpirvEmitterState& state, const MemoryInfo& mem) {
    throw std::runtime_error("PrepareMemoryResourceAccess not implemented");
}

MemoryResourceAccess PrepareStorageBufferResourceAccess(SpirvEmitterState& state, const MemoryInfo& mem, std::uint32_t variable, std::uint32_t pointerType) {
    throw std::runtime_error("PrepareStorageBufferResourceAccess not implemented");
}

std::uint32_t EmitMemoryElementIndex(SpirvEmitterState& state, const MemoryResourceAccess& access, std::uint32_t rawIndex) {
    throw std::runtime_error("EmitMemoryElementIndex not implemented");
}

std::uint32_t EmitMemoryElementInBounds(SpirvEmitterState& state, const MemoryResourceAccess& access, std::uint32_t index) {
    throw std::runtime_error("EmitMemoryElementInBounds not implemented");
}

std::uint32_t EmitMemoryElementPointer(SpirvEmitterState& state, const MemoryResourceAccess& access, std::uint32_t index) {
    throw std::runtime_error("EmitMemoryElementPointer not implemented");
}

std::uint32_t EmitStorageBufferElementPointer(SpirvEmitterState& state, const MemoryResourceAccess& access, std::uint32_t index, std::uint32_t pointerType) {
    throw std::runtime_error("EmitStorageBufferElementPointer not implemented");
}

std::uint32_t EmitTBufferBitcastU32ToI32(SpirvEmitterState& state, std::uint32_t value) {
    throw std::runtime_error("EmitTBufferBitcastU32ToI32 not implemented");
}

bool IsSignedFormatComponent(SpirvFormatComponentType type) {
    throw std::runtime_error("IsSignedFormatComponent not implemented");
}

std::uint32_t EmitUFloatToF32Bits(SpirvEmitterState& state, std::uint32_t raw, std::uint32_t bits) {
    throw std::runtime_error("EmitUFloatToF32Bits not implemented");
}

std::uint32_t NormalizeFormatComponent(SpirvEmitterState& state, const SpirvBufferFormatInfo& info, std::uint32_t component, std::uint32_t raw) {
    throw std::runtime_error("NormalizeFormatComponent not implemented");
}

void EmitDeviceAtomicMemoryBarrier(SpirvEmitterState& state) {
    throw std::runtime_error("EmitDeviceAtomicMemoryBarrier not implemented");
}

std::uint32_t EmitFloatAtomicReplacement(SpirvEmitterState& state, std::uint32_t old, std::uint32_t source, bool maxValue) {
    throw std::runtime_error("EmitFloatAtomicReplacement not implemented");
}

std::uint32_t EmitDsSwizzleTargetLane(SpirvEmitterState& state, std::uint32_t subid, std::uint32_t control) {
    throw std::runtime_error("EmitDsSwizzleTargetLane not implemented");
}

std::uint32_t EmitAndConstant(SpirvEmitterState& state, std::uint32_t value, std::uint32_t mask) {
    throw std::runtime_error("EmitAndConstant not implemented");
}

std::uint32_t EmitShiftRightConstant(SpirvEmitterState& state, std::uint32_t value, std::uint32_t shift) {
    throw std::runtime_error("EmitShiftRightConstant not implemented");
}

std::uint32_t EmitCompareU32Constant(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t value, std::uint32_t constant) {
    throw std::runtime_error("EmitCompareU32Constant not implemented");
}

std::uint32_t EmitSubConstantMinusU32(SpirvEmitterState& state, std::uint32_t constant, std::uint32_t value) {
    throw std::runtime_error("EmitSubConstantMinusU32 not implemented");
}

std::uint32_t EmitF32ToF16RtzBits(SpirvEmitterState& state, std::uint32_t f32) {
    throw std::runtime_error("EmitF32ToF16RtzBits not implemented");
}

std::uint32_t EmitMinMaxU32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    throw std::runtime_error("EmitMinMaxU32Value not implemented");
}

std::uint32_t EmitMinMaxI32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    throw std::runtime_error("EmitMinMaxI32Value not implemented");
}

F32Class EmitClassifyF32Bits(SpirvEmitterState& state, std::uint32_t bits) {
    throw std::runtime_error("EmitClassifyF32Bits not implemented");
}

F32Class EmitClassifyF32(SpirvEmitterState& state, std::uint32_t value) {
    throw std::runtime_error("EmitClassifyF32 not implemented");
}

std::uint32_t EmitClassMaskBitMatch(SpirvEmitterState& state, std::uint32_t mask, std::uint32_t bit, std::uint32_t classMatch) {
    throw std::runtime_error("EmitClassMaskBitMatch not implemented");
}

std::uint32_t EmitClassMaskF32(SpirvEmitterState& state, std::uint32_t value, std::uint32_t mask) {
    throw std::runtime_error("EmitClassMaskF32 not implemented");
}

std::uint32_t EmitMinMaxF32Value(SpirvEmitterState& state, std::uint32_t lhs, std::uint32_t rhs, bool maxValue) {
    throw std::runtime_error("EmitMinMaxF32Value not implemented");
}

std::uint32_t EmitFlushF32DenormToSignedZero(SpirvEmitterState& state, std::uint32_t value) {
    throw std::runtime_error("EmitFlushF32DenormToSignedZero not implemented");
}

std::uint32_t EmitTrigCycleF32(SpirvEmitterState& state, std::uint32_t src, bool preserveSignedZero) {
    throw std::runtime_error("EmitTrigCycleF32 not implemented");
}

std::uint32_t EmitF16BitsToF32(SpirvEmitterState& state, std::uint32_t bits) {
    throw std::runtime_error("EmitF16BitsToF32 not implemented");
}

void EmitProgram(SpirvEmitterState& state) {
    throw std::runtime_error("EmitProgram not implemented");
}

void DefineGetBdaPointer(SpirvEmitterState& state) {
    throw std::runtime_error("DefineGetBdaPointer not implemented");
}

void EmitLabel(SpirvEmitterState& state, std::uint32_t label) {
    throw std::runtime_error("EmitLabel not implemented");
}

std::uint32_t Unary(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t type, std::uint32_t value) {
    throw std::runtime_error("Unary not implemented");
}

std::uint32_t Binary(SpirvEmitterState& state, std::uint32_t opcode, std::uint32_t type, std::uint32_t lhs, std::uint32_t rhs) {
    throw std::runtime_error("Binary not implemented");
}

std::uint32_t Select(SpirvEmitterState& state, std::uint32_t type, std::uint32_t condition, std::uint32_t trueValue, std::uint32_t falseValue) {
    throw std::runtime_error("Select not implemented");
}

std::uint32_t EmitMeshDrawParameter(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitMeshDrawParameter not implemented");
}

std::uint32_t EmitGetTessellationAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetTessellationAttribute not implemented");
}

void EmitSetTessellationAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetTessellationAttribute not implemented");
}

std::uint32_t EmitGetUserData(SpirvEmitterState& state, ScalarReg reg) {
    throw std::runtime_error("EmitGetUserData not implemented");
}

std::uint32_t EmitGetBuiltin(SpirvValueEmitContext& ctx, const IrValue* kind, const IrValue* index) {
    throw std::runtime_error("EmitGetBuiltin not implemented");
}

std::uint32_t EmitGetAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetAttribute not implemented");
}

std::uint32_t EmitGetInterpolationParameter(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetInterpolationParameter not implemented");
}

void EmitSetAttribute(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetAttribute not implemented");
}

std::uint32_t EmitGetShaderBase(SpirvValueEmitContext& ctx) {
    throw std::runtime_error("EmitGetShaderBase not implemented");
}

void EmitTessellationBase(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitTessellationBase not implemented");
}

}
