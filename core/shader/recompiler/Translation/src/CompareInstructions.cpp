#include "Translation/CompareInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateCompareInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateCompareInstruction not implemented");
}

void TranslationContext::emitCompareResult(const RdnaInstruction& inst, IrU1 value, bool scalar, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitCompareResult not implemented");
}

void TranslationContext::emitCompareConstant(const RdnaInstruction& inst, bool value, bool scalar, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitCompareConstant not implemented");
}

void TranslationContext::emitIntegerCompare(const RdnaInstruction& inst, IrOpcode opcode, IrType type, bool scalar, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitIntegerCompare not implemented");
}

void TranslationContext::emitInteger16Compare(const RdnaInstruction& inst, IrOpcode opcode, bool signedValue, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitInteger16Compare not implemented");
}

void TranslationContext::emitFloatCompare(const RdnaInstruction& inst, IrOpcode opcode, bool half, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitFloatCompare not implemented");
}

void TranslationContext::emitFloatOrderedCompare(const RdnaInstruction& inst, bool ordered) {
    throw std::runtime_error("TranslationContext::emitFloatOrderedCompare not implemented");
}

void TranslationContext::emitFloatClassCompare(const RdnaInstruction& inst, bool cmpx) {
    throw std::runtime_error("TranslationContext::emitFloatClassCompare not implemented");
}

void TranslateCompareInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateCompareInstruction not implemented");
}

}
