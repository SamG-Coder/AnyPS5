#include "../../include/SpirvBackend/SpirvEmitterInstructions.hpp"
#include <../../../../../../.cache/toolchains/winlibs-gcc15-r6/mingw64/include/c++/15.2.0/stdexcept>
#include <../../../../../../.cache/toolchains/winlibs-gcc15-r6/mingw64/include/c++/15.2.0/string>

namespace ShaderRecompiler {
namespace {

[[noreturn]] void ThrowNotImplemented(const char* functionName) {
    throw std::runtime_error(std::string("ShaderRecompiler::") + functionName + " not implemented");
}

}

std::uint32_t EmitReadConst(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitReadConstBuffer(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitGetSrtResource(SpirvValueEmitContext&) {
    ThrowNotImplemented(__func__);
}

void EmitGetBufferResource(SpirvValueEmitContext&) {
    ThrowNotImplemented(__func__);
}

void EmitGetAddressResource(SpirvValueEmitContext&) {
    ThrowNotImplemented(__func__);
}

void EmitGetScratchResource(SpirvValueEmitContext&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadAddressU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadAddressU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadAddressU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreAddressU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadBufferU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitStoreBufferU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicCmpSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSwap64(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicIAdd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicISub32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicUMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicSMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicUMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicAnd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicOr32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicOr64(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicXor32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicFMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitBufferAtomicFMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitLoadSharedU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU8(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU16(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x2(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x3(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitWriteSharedU32x4(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitSharedAtomicFMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

void EmitSharedAtomicFMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSwap32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicIAdd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicISub32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicInc32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicDec32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicUMin32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicSMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicUMax32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicAnd32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicOr32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitSharedAtomicXor32(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitDataAppend(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

std::uint32_t EmitDataConsume(SpirvValueEmitContext&, const IrValue&) {
    ThrowNotImplemented(__func__);
}

}
