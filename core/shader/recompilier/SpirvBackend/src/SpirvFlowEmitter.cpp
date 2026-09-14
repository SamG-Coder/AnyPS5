#include "SpirvBackend/SpirvFlowEmitter.hpp"
#include "SpirvBackend/SpirvEmitterInstructions.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void EmitControlFlow(SpirvModule& module, const IrProgram& program) {
    throw std::runtime_error("EmitControlFlow not implemented");
}

void EmitControlFlow(SpirvValueEmitContext& context, StructuredFunctionState& functionState, const IrProgram& program) {
    throw std::runtime_error("EmitControlFlow not implemented");
}

void EmitVoid(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitVoid not implemented");
}

void EmitBarrier(SpirvEmitterState& state) {
    throw std::runtime_error("EmitBarrier not implemented");
}

void EmitUnreachable(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitUnreachable not implemented");
}

void EmitReference(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitReference not implemented");
}

void EmitReferenceU32(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitReferenceU32 not implemented");
}

void EmitControlNop(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitControlNop not implemented");
}

void EmitWaitcnt(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitWaitcnt not implemented");
}

void EmitSendmsg(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitSendmsg not implemented");
}

void EmitTtraceData(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitTtraceData not implemented");
}

void EmitInstPrefetch(SpirvValueEmitContext& context) {
    throw std::runtime_error("EmitInstPrefetch not implemented");
}

void EmitPhi(SpirvValueEmitContext& ctx, const IrValue& inst) {
    throw std::runtime_error("EmitPhi not implemented");
}

}
