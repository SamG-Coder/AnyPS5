#include "SpirvBackend/SpirvAluEmitter.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void EmitAluValue(SpirvModule& module, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitAluValue not implemented");
}

void EmitAluValue(SpirvValueEmitContext& context, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitAluValue not implemented");
}

std::uint32_t EmitConvertU16U32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertU16U32 not implemented");
}

std::uint32_t EmitConvertU8U32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertU8U32 not implemented");
}

std::uint32_t EmitConvertF16F32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertF16F32 not implemented");
}

std::uint32_t EmitConvertS32F32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertS32F32 not implemented");
}

std::uint32_t EmitConvertU32F32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertU32F32 not implemented");
}

std::uint32_t EmitConvertF32S32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertF32S32 not implemented");
}

std::uint32_t EmitCompositeExtractU64(SpirvEmitterState& state, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitCompositeExtractU64 not implemented");
}

std::uint32_t EmitPackFloat2x16Rtz(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitPackFloat2x16Rtz not implemented");
}

std::uint32_t EmitFPSaturate32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPSaturate32 not implemented");
}

std::uint32_t EmitIAdd64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIAdd64 not implemented");
}

std::uint32_t EmitISub64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitISub64 not implemented");
}

std::uint32_t EmitIMul64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIMul64 not implemented");
}

std::uint32_t EmitSMulHi(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSMulHi not implemented");
}

std::uint32_t EmitUMulHi(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUMulHi not implemented");
}

std::uint32_t EmitIAbs32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitIAbs32 not implemented");
}

std::uint32_t EmitShiftLeftLogical64(SpirvValueEmitContext& ctx, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitShiftLeftLogical64 not implemented");
}

std::uint32_t EmitShiftRightLogical64(SpirvValueEmitContext& ctx, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitShiftRightLogical64 not implemented");
}

std::uint32_t EmitShiftRightArithmetic64(SpirvValueEmitContext& ctx, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitShiftRightArithmetic64 not implemented");
}

std::uint32_t EmitBitwiseAnd64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitBitwiseAnd64 not implemented");
}

std::uint32_t EmitBitCount64(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCount64 not implemented");
}

std::uint32_t EmitFindILsb32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFindILsb32 not implemented");
}

std::uint32_t EmitFindUMsb32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFindUMsb32 not implemented");
}

std::uint32_t EmitFindUMsb64(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFindUMsb64 not implemented");
}

std::uint32_t EmitSMin32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSMin32 not implemented");
}

std::uint32_t EmitSMax32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSMax32 not implemented");
}

std::uint32_t EmitUMin32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUMin32 not implemented");
}

std::uint32_t EmitUMax32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUMax32 not implemented");
}

std::uint32_t EmitSMinTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSMinTri32 not implemented");
}

std::uint32_t EmitSMaxTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSMaxTri32 not implemented");
}

std::uint32_t EmitUMinTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitUMinTri32 not implemented");
}

std::uint32_t EmitUMaxTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitUMaxTri32 not implemented");
}

std::uint32_t EmitSMedTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSMedTri32 not implemented");
}

std::uint32_t EmitUMedTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitUMedTri32 not implemented");
}

std::uint32_t EmitIEqual64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIEqual64 not implemented");
}

std::uint32_t EmitINotEqual64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitINotEqual64 not implemented");
}

std::uint32_t EmitULessThan64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitULessThan64 not implemented");
}

std::uint32_t EmitSLessThan64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSLessThan64 not implemented");
}

std::uint32_t EmitUGreaterThan64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUGreaterThan64 not implemented");
}

std::uint32_t EmitFPIsNan32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPIsNan32 not implemented");
}

std::uint32_t EmitFPMin32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPMin32 not implemented");
}

std::uint32_t EmitFPMax32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPMax32 not implemented");
}

std::uint32_t EmitFPMinTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitFPMinTri32 not implemented");
}

std::uint32_t EmitFPMaxTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitFPMaxTri32 not implemented");
}

std::uint32_t EmitFPMedTri32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitFPMedTri32 not implemented");
}

std::uint32_t EmitFPRecip32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPRecip32 not implemented");
}

std::uint32_t EmitFPRecipIFlag32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPRecipIFlag32 not implemented");
}

std::uint32_t EmitFPRecipSqrt32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPRecipSqrt32 not implemented");
}

std::uint32_t EmitFPSqrt(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPSqrt not implemented");
}

std::uint32_t EmitFPExp2(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPExp2 not implemented");
}

std::uint32_t EmitFPLog2(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPLog2 not implemented");
}

std::uint32_t EmitFPLdexp(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPLdexp not implemented");
}

std::uint32_t EmitFPSin(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPSin not implemented");
}

std::uint32_t EmitFPCos(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPCos not implemented");
}

std::uint32_t EmitIdentity(SpirvValueEmitContext& ctx, std::uint32_t value) {
    throw std::runtime_error("EmitIdentity not implemented");
}

std::uint32_t EmitUndefU1(SpirvEmitterState& state, const IrValue& inst) {
    throw std::runtime_error("EmitUndefU1 not implemented");
}

std::uint32_t EmitDppMoveU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitDppMoveU32 not implemented");
}

std::uint32_t EmitDppUpdateU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitDppUpdateU32 not implemented");
}

std::uint32_t EmitWqmU64(SpirvEmitterState& state, std::uint32_t value) {
    throw std::runtime_error("EmitWqmU64 not implemented");
}

std::uint32_t EmitLaneId(SpirvEmitterState& state) {
    throw std::runtime_error("EmitLaneId not implemented");
}

std::uint32_t EmitBallot(SpirvValueEmitContext& ctx, const IrValue* predicate) {
    throw std::runtime_error("EmitBallot not implemented");
}

std::uint32_t EmitReadFirstLane(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitReadFirstLane not implemented");
}

std::uint32_t EmitReadLane(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitReadLane not implemented");
}

std::uint32_t EmitWriteLane(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteLane not implemented");
}

std::uint32_t EmitPermlane16U32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitPermlane16U32 not implemented");
}

std::uint32_t EmitBpermuteU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBpermuteU32 not implemented");
}

std::uint32_t EmitSwizzleU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSwizzleU32 not implemented");
}

std::uint32_t EmitBitCastU16F16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCastU16F16 not implemented");
}

std::uint32_t EmitBitCastF32U32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCastF32U32 not implemented");
}

std::uint32_t EmitConvertF32U32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertF32U32 not implemented");
}

std::uint32_t EmitCompositeConstructU64(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitCompositeConstructU64 not implemented");
}

std::uint32_t EmitCompositeConstructU32x2(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitCompositeConstructU32x2 not implemented");
}

std::uint32_t EmitCompositeConstructU32x3(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitCompositeConstructU32x3 not implemented");
}

std::uint32_t EmitCompositeConstructF32x2(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitCompositeConstructF32x2 not implemented");
}

std::uint32_t EmitCompositeConstructU32x4(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2, std::uint32_t arg3) {
    throw std::runtime_error("EmitCompositeConstructU32x4 not implemented");
}

std::uint32_t EmitBitFieldInsert(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2, std::uint32_t arg3) {
    throw std::runtime_error("EmitBitFieldInsert not implemented");
}

std::uint32_t EmitBitFieldUExtract(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitBitFieldUExtract not implemented");
}

std::uint32_t EmitBitFieldSExtract(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitBitFieldSExtract not implemented");
}

std::uint32_t EmitSelectU1(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSelectU1 not implemented");
}

std::uint32_t EmitSelectU32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSelectU32 not implemented");
}

std::uint32_t EmitSelectF32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSelectF32 not implemented");
}

std::uint32_t EmitIAdd32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIAdd32 not implemented");
}

std::uint32_t EmitISub32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitISub32 not implemented");
}

std::uint32_t EmitIMul32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIMul32 not implemented");
}

std::uint32_t EmitUDiv32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUDiv32 not implemented");
}

std::uint32_t EmitIAddCarry32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIAddCarry32 not implemented");
}

std::uint32_t EmitShiftLeftLogical32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitShiftLeftLogical32 not implemented");
}

std::uint32_t EmitShiftRightLogical32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitShiftRightLogical32 not implemented");
}

std::uint32_t EmitShiftRightArithmetic32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitShiftRightArithmetic32 not implemented");
}

std::uint32_t EmitBitwiseAnd32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitBitwiseAnd32 not implemented");
}

std::uint32_t EmitBitwiseOr32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitBitwiseOr32 not implemented");
}

std::uint32_t EmitBitwiseXor32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitBitwiseXor32 not implemented");
}

std::uint32_t EmitBitwiseNot32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitwiseNot32 not implemented");
}

std::uint32_t EmitBitReverse32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitReverse32 not implemented");
}

std::uint32_t EmitBitCount32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCount32 not implemented");
}

std::uint32_t EmitSLessThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSLessThan32 not implemented");
}

std::uint32_t EmitULessThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitULessThan32 not implemented");
}

std::uint32_t EmitIEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitIEqual32 not implemented");
}

std::uint32_t EmitSLessThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSLessThanEqual32 not implemented");
}

std::uint32_t EmitULessThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitULessThanEqual32 not implemented");
}

std::uint32_t EmitSGreaterThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSGreaterThan32 not implemented");
}

std::uint32_t EmitUGreaterThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUGreaterThan32 not implemented");
}

std::uint32_t EmitINotEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitINotEqual32 not implemented");
}

std::uint32_t EmitSGreaterThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitSGreaterThanEqual32 not implemented");
}

std::uint32_t EmitUGreaterThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitUGreaterThanEqual32 not implemented");
}

std::uint32_t EmitLogicalOr(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitLogicalOr not implemented");
}

std::uint32_t EmitLogicalAnd(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitLogicalAnd not implemented");
}

std::uint32_t EmitLogicalXor(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitLogicalXor not implemented");
}

std::uint32_t EmitLogicalNot(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitLogicalNot not implemented");
}

std::uint32_t EmitFPOrdEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdEqual32 not implemented");
}

std::uint32_t EmitFPUnordEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordEqual32 not implemented");
}

std::uint32_t EmitFPOrdNotEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdNotEqual32 not implemented");
}

std::uint32_t EmitFPUnordNotEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordNotEqual32 not implemented");
}

std::uint32_t EmitFPOrdLessThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdLessThan32 not implemented");
}

std::uint32_t EmitFPUnordLessThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordLessThan32 not implemented");
}

std::uint32_t EmitFPOrdGreaterThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdGreaterThan32 not implemented");
}

std::uint32_t EmitFPUnordGreaterThan32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordGreaterThan32 not implemented");
}

std::uint32_t EmitFPOrdLessThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdLessThanEqual32 not implemented");
}

std::uint32_t EmitFPUnordLessThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordLessThanEqual32 not implemented");
}

std::uint32_t EmitFPOrdGreaterThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPOrdGreaterThanEqual32 not implemented");
}

std::uint32_t EmitFPUnordGreaterThanEqual32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPUnordGreaterThanEqual32 not implemented");
}

std::uint32_t EmitFPAdd32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPAdd32 not implemented");
}

std::uint32_t EmitFPSub32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPSub32 not implemented");
}

std::uint32_t EmitFPMul32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitFPMul32 not implemented");
}

std::uint32_t EmitAddU32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitAddU32 not implemented");
}

std::uint32_t EmitTBufferSelectF32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitTBufferSelectF32 not implemented");
}

std::uint32_t EmitSelectValueU32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitSelectValueU32 not implemented");
}

std::uint32_t EmitOrU32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitOrU32 not implemented");
}

std::uint32_t EmitBitcastF32ToU32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitcastF32ToU32 not implemented");
}

std::uint32_t EmitBitcastU32ToF32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitcastU32ToF32 not implemented");
}

std::uint32_t EmitAndU32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitAndU32 not implemented");
}

std::uint32_t EmitLogicalAndBool(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitLogicalAndBool not implemented");
}

std::uint32_t EmitLogicalOrBool(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1) {
    throw std::runtime_error("EmitLogicalOrBool not implemented");
}

std::uint32_t EmitLogicalNotBool(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitLogicalNotBool not implemented");
}

std::uint32_t EmitTruncF32Value(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitTruncF32Value not implemented");
}

std::uint32_t EmitFNegateValue(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFNegateValue not implemented");
}

std::uint32_t EmitFAbsValue(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFAbsValue not implemented");
}

std::uint32_t EmitPackHalf2x16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitPackHalf2x16 not implemented");
}

std::uint32_t EmitPackSnorm2x16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitPackSnorm2x16 not implemented");
}

std::uint32_t EmitPackUnorm2x16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitPackUnorm2x16 not implemented");
}

std::uint32_t EmitFPFma32(SpirvEmitterState& state, std::uint32_t arg0, std::uint32_t arg1, std::uint32_t arg2) {
    throw std::runtime_error("EmitFPFma32 not implemented");
}

std::uint32_t EmitFPRoundEven32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPRoundEven32 not implemented");
}

std::uint32_t EmitFPFloor32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPFloor32 not implemented");
}

std::uint32_t EmitFPCeil32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPCeil32 not implemented");
}

std::uint32_t EmitFPTrunc32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPTrunc32 not implemented");
}

std::uint32_t EmitFPFract32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPFract32 not implemented");
}

std::uint32_t EmitBitCastF16U16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCastF16U16 not implemented");
}

std::uint32_t EmitConvertU32U16(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertU32U16 not implemented");
}

std::uint32_t EmitConvertU32U8(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitConvertU32U8 not implemented");
}

std::uint32_t EmitBitCastU32F32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitBitCastU32F32 not implemented");
}

std::uint32_t EmitConvertF32F16(SpirvEmitterState& state, std::uint32_t bits) {
    throw std::runtime_error("EmitConvertF32F16 not implemented");
}

std::uint32_t EmitCompositeExtractU32x2(SpirvEmitterState& state, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitCompositeExtractU32x2 not implemented");
}

std::uint32_t EmitCompositeExtractU32x3(SpirvEmitterState& state, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitCompositeExtractU32x3 not implemented");
}

std::uint32_t EmitCompositeExtractU32x4(SpirvEmitterState& state, std::uint32_t arg0, const IrValue* arg1) {
    throw std::runtime_error("EmitCompositeExtractU32x4 not implemented");
}

std::uint32_t EmitFPAbs32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPAbs32 not implemented");
}

std::uint32_t EmitFPNeg32(SpirvEmitterState& state, std::uint32_t arg0) {
    throw std::runtime_error("EmitFPNeg32 not implemented");
}

std::uint32_t EmitFPCmpClass32(SpirvEmitterState& state, std::uint32_t value, std::uint32_t mask) {
    throw std::runtime_error("EmitFPCmpClass32 not implemented");
}

std::uint32_t EmitUndefU8(SpirvEmitterState& state, const IrValue& inst) {
    throw std::runtime_error("EmitUndefU8 not implemented");
}

std::uint32_t EmitUndefU16(SpirvEmitterState& state, const IrValue& inst) {
    throw std::runtime_error("EmitUndefU16 not implemented");
}

std::uint32_t EmitUndefU32(SpirvEmitterState& state, const IrValue& inst) {
    throw std::runtime_error("EmitUndefU32 not implemented");
}

std::uint32_t EmitUndefU64(SpirvEmitterState& state, const IrValue& inst) {
    throw std::runtime_error("EmitUndefU64 not implemented");
}

void EmitGetThreadBitScalarRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetThreadBitScalarRegister not implemented");
}

void EmitSetThreadBitScalarRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetThreadBitScalarRegister not implemented");
}

void EmitGetScalarMaskTag(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetScalarMaskTag not implemented");
}

void EmitSetScalarMaskTag(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetScalarMaskTag not implemented");
}

void EmitGetScalarRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetScalarRegister not implemented");
}

void EmitSetScalarRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetScalarRegister not implemented");
}

void EmitGetVectorRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetVectorRegister not implemented");
}

void EmitSetVectorRegister(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetVectorRegister not implemented");
}

void EmitGetGotoVariable(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetGotoVariable not implemented");
}

void EmitSetGotoVariable(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetGotoVariable not implemented");
}

void EmitGetScc(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetScc not implemented");
}

void EmitSetScc(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetScc not implemented");
}

void EmitGetExec(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetExec not implemented");
}

void EmitSetExec(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetExec not implemented");
}

void EmitGetExecLo(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetExecLo not implemented");
}

void EmitSetExecLo(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetExecLo not implemented");
}

void EmitGetExecHi(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetExecHi not implemented");
}

void EmitSetExecHi(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetExecHi not implemented");
}

void EmitGetVcc(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetVcc not implemented");
}

void EmitSetVcc(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetVcc not implemented");
}

void EmitGetVccLo(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetVccLo not implemented");
}

void EmitSetVccLo(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetVccLo not implemented");
}

void EmitGetVccHi(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetVccHi not implemented");
}

void EmitSetVccHi(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetVccHi not implemented");
}

void EmitGetM0(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitGetM0 not implemented");
}

void EmitSetM0(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSetM0 not implemented");
}

}
