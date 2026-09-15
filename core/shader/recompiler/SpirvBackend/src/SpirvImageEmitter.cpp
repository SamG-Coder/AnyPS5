#include "SpirvBackend/SpirvImageEmitter.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void EmitImageOperation(SpirvModule& module, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitImageOperation not implemented");
}

void EmitImageOperation(SpirvValueEmitContext& context, const IrValue& value, std::uint32_t resultId) {
    throw std::runtime_error("EmitImageOperation not implemented");
}

void EmitImage(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImage not implemented");
}

void EmitGetImageResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetImageResource not implemented");
}

void EmitGetSamplerResource(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitGetSamplerResource not implemented");
}

void EmitMakeImageAddress(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitMakeImageAddress not implemented");
}

void EmitImageQueryDimensions(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageQueryDimensions not implemented");
}

void EmitImageQueryLod(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageQueryLod not implemented");
}

void EmitImageRead(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageRead not implemented");
}

void EmitImageWrite(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageWrite not implemented");
}

void EmitImageSampleRaw(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageSampleRaw not implemented");
}

void EmitImageGatherRaw(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageGatherRaw not implemented");
}

void EmitImageAtomicSwap32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicSwap32 not implemented");
}

void EmitImageAtomicIAdd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicIAdd32 not implemented");
}

void EmitImageAtomicUMin32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicUMin32 not implemented");
}

void EmitImageAtomicUMax32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicUMax32 not implemented");
}

void EmitImageAtomicAnd32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicAnd32 not implemented");
}

void EmitImageAtomicOr32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicOr32 not implemented");
}

void EmitImageAtomicXor32(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitImageAtomicXor32 not implemented");
}

}
