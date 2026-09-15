#include "SpirvBackend/SpirvMemoryEmitter.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void EmitMemoryOperation(SpirvModule& module, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitMemoryOperation not implemented");
}

void EmitMemoryOperation(SpirvValueEmitContext& context, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitMemoryOperation not implemented");
}

void EmitLoadMemory(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadMemory not implemented");
}

void EmitStoreMemory(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreMemory not implemented");
}

std::uint32_t EmitAtomic32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitAtomic32 not implemented");
}

std::uint32_t EmitBufferAtomic64(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomic64 not implemented");
}

std::uint32_t EmitBufferFloatAtomic(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferFloatAtomic not implemented");
}

void EmitSharedFloatAtomic(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedFloatAtomic not implemented");
}

std::uint32_t EmitSharedIncDec(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedIncDec not implemented");
}

std::uint32_t EmitAppendConsume(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitAppendConsume not implemented");
}

std::uint32_t EmitReadConst(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitReadConst not implemented");
}

void EmitReadConstBuffer(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitReadConstBuffer not implemented");
}

void EmitGetSrtResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetSrtResource not implemented");
}

void EmitGetBufferResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetBufferResource not implemented");
}

void EmitGetAddressResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetAddressResource not implemented");
}

void EmitGetScratchResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetScratchResource not implemented");
}

void EmitLoadAddressU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadAddressU8 not implemented");
}

void EmitLoadAddressU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadAddressU16 not implemented");
}

void EmitLoadAddressU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadAddressU32 not implemented");
}

void EmitStoreAddressU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreAddressU8 not implemented");
}

void EmitStoreAddressU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreAddressU16 not implemented");
}

void EmitStoreAddressU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreAddressU32 not implemented");
}

void EmitLoadBufferU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU8 not implemented");
}

void EmitLoadBufferU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU16 not implemented");
}

void EmitLoadBufferU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU32 not implemented");
}

void EmitLoadBufferU32x2(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU32x2 not implemented");
}

void EmitLoadBufferU32x3(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU32x3 not implemented");
}

void EmitLoadBufferU32x4(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadBufferU32x4 not implemented");
}

void EmitStoreBufferU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU8 not implemented");
}

void EmitStoreBufferU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU16 not implemented");
}

void EmitStoreBufferU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU32 not implemented");
}

void EmitStoreBufferU32x2(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU32x2 not implemented");
}

void EmitStoreBufferU32x3(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU32x3 not implemented");
}

void EmitStoreBufferU32x4(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitStoreBufferU32x4 not implemented");
}

std::uint32_t EmitBufferAtomicSwap32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicSwap32 not implemented");
}

std::uint32_t EmitBufferAtomicCmpSwap32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicCmpSwap32 not implemented");
}

std::uint32_t EmitBufferAtomicSwap64(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicSwap64 not implemented");
}

std::uint32_t EmitBufferAtomicIAdd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicIAdd32 not implemented");
}

std::uint32_t EmitBufferAtomicISub32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicISub32 not implemented");
}

std::uint32_t EmitBufferAtomicSMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicSMin32 not implemented");
}

std::uint32_t EmitBufferAtomicUMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicUMin32 not implemented");
}

std::uint32_t EmitBufferAtomicSMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicSMax32 not implemented");
}

std::uint32_t EmitBufferAtomicUMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicUMax32 not implemented");
}

std::uint32_t EmitBufferAtomicAnd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicAnd32 not implemented");
}

std::uint32_t EmitBufferAtomicOr32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicOr32 not implemented");
}

std::uint32_t EmitBufferAtomicOr64(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicOr64 not implemented");
}

std::uint32_t EmitBufferAtomicXor32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicXor32 not implemented");
}

std::uint32_t EmitBufferAtomicFMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicFMin32 not implemented");
}

std::uint32_t EmitBufferAtomicFMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitBufferAtomicFMax32 not implemented");
}

void EmitLoadSharedU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU8 not implemented");
}

void EmitLoadSharedU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU16 not implemented");
}

void EmitLoadSharedU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU32 not implemented");
}

void EmitLoadSharedU32x2(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU32x2 not implemented");
}

void EmitLoadSharedU32x3(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU32x3 not implemented");
}

void EmitLoadSharedU32x4(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitLoadSharedU32x4 not implemented");
}

void EmitWriteSharedU8(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU8 not implemented");
}

void EmitWriteSharedU16(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU16 not implemented");
}

void EmitWriteSharedU32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU32 not implemented");
}

void EmitWriteSharedU32x2(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU32x2 not implemented");
}

void EmitWriteSharedU32x3(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU32x3 not implemented");
}

void EmitWriteSharedU32x4(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitWriteSharedU32x4 not implemented");
}

void EmitSharedAtomicFMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicFMin32 not implemented");
}

void EmitSharedAtomicFMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicFMax32 not implemented");
}

std::uint32_t EmitSharedAtomicSwap32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicSwap32 not implemented");
}

std::uint32_t EmitSharedAtomicIAdd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicIAdd32 not implemented");
}

std::uint32_t EmitSharedAtomicISub32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicISub32 not implemented");
}

std::uint32_t EmitSharedAtomicInc32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicInc32 not implemented");
}

std::uint32_t EmitSharedAtomicDec32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicDec32 not implemented");
}

std::uint32_t EmitSharedAtomicSMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicSMin32 not implemented");
}

std::uint32_t EmitSharedAtomicUMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicUMin32 not implemented");
}

std::uint32_t EmitSharedAtomicSMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicSMax32 not implemented");
}

std::uint32_t EmitSharedAtomicUMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicUMax32 not implemented");
}

std::uint32_t EmitSharedAtomicAnd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicAnd32 not implemented");
}

std::uint32_t EmitSharedAtomicOr32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicOr32 not implemented");
}

std::uint32_t EmitSharedAtomicXor32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitSharedAtomicXor32 not implemented");
}

std::uint32_t EmitDataAppend(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitDataAppend not implemented");
}

std::uint32_t EmitDataConsume(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitDataConsume not implemented");
}

}
