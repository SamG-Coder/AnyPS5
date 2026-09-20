#include "SpirvBackend/SpirvModuleEmitter.hpp"
#include "SpirvBackend/SpirvEmitterHelpers.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

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
