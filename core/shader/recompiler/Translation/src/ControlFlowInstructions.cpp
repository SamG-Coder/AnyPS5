#include "Translation/ControlFlowInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateControlFlowInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg) {
    throw std::runtime_error("TranslateControlFlowInstruction not implemented");
}

void TranslationContext::sSubvectorLoop(const RdnaInstruction& inst, bool begin) {
    throw std::runtime_error("TranslationContext::sSubvectorLoop not implemented");
}

void TranslationContext::sSaveexec(const RdnaInstruction& inst, IrOpcode operation, bool negateExec, bool negateSource, bool write64) {
    throw std::runtime_error("TranslationContext::sSaveexec not implemented");
}

void TranslationContext::addU32(const RdnaInstruction& inst, bool vector, bool useCarryIn) {
    throw std::runtime_error("TranslationContext::addU32 not implemented");
}

void TranslationContext::subU32(const RdnaInstruction& inst, bool vector, bool reverse) {
    throw std::runtime_error("TranslationContext::subU32 not implemented");
}

void TranslationContext::subbU32(const RdnaInstruction& inst, bool vector, bool reverse) {
    throw std::runtime_error("TranslationContext::subbU32 not implemented");
}

void TranslationContext::sAbsdiffI32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sAbsdiffI32 not implemented");
}

void TranslationContext::sAddSubI32(const RdnaInstruction& inst, bool subtract) {
    throw std::runtime_error("TranslationContext::sAddSubI32 not implemented");
}

void TranslationContext::sLshlAddU32(const RdnaInstruction& inst, std::uint32_t shiftAmount) {
    throw std::runtime_error("TranslationContext::sLshlAddU32 not implemented");
}

void TranslationContext::scalarMinMax32(const RdnaInstruction& inst, IrOpcode valueOpcode, IrOpcode compareOpcode) {
    throw std::runtime_error("TranslationContext::scalarMinMax32 not implemented");
}

void TranslationContext::emitControlNop() {
    throw std::runtime_error("TranslationContext::emitControlNop not implemented");
}

void TranslationContext::emitWaitcnt() {
    throw std::runtime_error("TranslationContext::emitWaitcnt not implemented");
}

void TranslationContext::sBarrier() {
    throw std::runtime_error("TranslationContext::sBarrier not implemented");
}

void TranslationContext::sSendmsg(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sSendmsg not implemented");
}

void TranslationContext::sTtracedata() {
    throw std::runtime_error("TranslationContext::sTtracedata not implemented");
}

void TranslationContext::sInstPrefetch() {
    throw std::runtime_error("TranslationContext::sInstPrefetch not implemented");
}

void TranslationContext::sGetpcB64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sGetpcB64 not implemented");
}

void TranslationContext::sCselectB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sCselectB32 not implemented");
}

void TranslationContext::scalarSelect64(const RdnaInstruction& inst, const RdnaOperand& falseSource) {
    throw std::runtime_error("TranslationContext::scalarSelect64 not implemented");
}

void TranslationContext::movB32(const RdnaInstruction& inst, bool applyFloatModifiers) {
    throw std::runtime_error("TranslationContext::movB32 not implemented");
}

void TranslationContext::sMovB64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sMovB64 not implemented");
}

void TranslationContext::sWqm(const RdnaInstruction& inst, bool wide) {
    throw std::runtime_error("TranslationContext::sWqm not implemented");
}

void TranslationContext::vMovrelsB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vMovrelsB32 not implemented");
}

void TranslationContext::vMovreldB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vMovreldB32 not implemented");
}

void TranslationContext::vReadfirstlaneB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vReadfirstlaneB32 not implemented");
}

void TranslationContext::vReadlaneB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vReadlaneB32 not implemented");
}

void TranslationContext::vWritelaneB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vWritelaneB32 not implemented");
}

void TranslationContext::vPermlane16B32(const RdnaInstruction& inst, bool x16) {
    throw std::runtime_error("TranslationContext::vPermlane16B32 not implemented");
}

void TranslateControlFlowInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateControlFlowInstruction not implemented");
}

}
