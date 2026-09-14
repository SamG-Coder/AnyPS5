#include "Translation/ConvertInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateConvertInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateConvertInstruction not implemented");
}

IrF32 TranslationContext::selectF32(IrU1 condition, IrF32 trueValue, IrF32 falseValue) {
    throw std::runtime_error("TranslationContext::selectF32 not implemented");
}

IrU32 TranslationContext::convertF32ToU32Saturated(IrF32 value, float upperBound, float safeUpper, std::uint32_t highResult) {
    throw std::runtime_error("TranslationContext::convertF32ToU32Saturated not implemented");
}

IrU32 TranslationContext::convertF32ToI32Saturated(IrF32 value, float lowerBound, float upperBound, float safeUpper, std::uint32_t lowerResult, std::uint32_t upperResult) {
    throw std::runtime_error("TranslationContext::convertF32ToI32Saturated not implemented");
}

IrU32 TranslationContext::packU16Lanes(IrU32 low, IrU32 high) {
    throw std::runtime_error("TranslationContext::packU16Lanes not implemented");
}

void TranslationContext::vCvtF32Ubyte(const RdnaInstruction& inst, std::uint32_t byteIndex) {
    throw std::runtime_error("TranslationContext::vCvtF32Ubyte not implemented");
}

void TranslationContext::vCvtF32U32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtF32U32 not implemented");
}

void TranslationContext::vCvtF32I32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtF32I32 not implemented");
}

void TranslationContext::vCvtU32F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtU32F32 not implemented");
}

void TranslationContext::vCvtI32F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtI32F32 not implemented");
}

void TranslationContext::vCvtF16F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtF16F32 not implemented");
}

void TranslationContext::vCvtF32F16(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtF32F16 not implemented");
}

void TranslationContext::vCvtF1616(const RdnaInstruction& inst, bool signedValue) {
    throw std::runtime_error("TranslationContext::vCvtF1616 not implemented");
}

void TranslationContext::vCvt16F16(const RdnaInstruction& inst, bool signedValue) {
    throw std::runtime_error("TranslationContext::vCvt16F16 not implemented");
}

void TranslationContext::vCvtRpiI32F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtRpiI32F32 not implemented");
}

void TranslationContext::vCvtFlrI32F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtFlrI32F32 not implemented");
}

void TranslationContext::vFrexpExpI32F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vFrexpExpI32F32 not implemented");
}

void TranslationContext::vCvtOffF32I4(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtOffF32I4 not implemented");
}

void TranslationContext::vCvtPkrtzF16F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtPkrtzF16F32 not implemented");
}

void TranslationContext::vCvtPknormF32(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::vCvtPknormF32 not implemented");
}

void TranslationContext::vCvtPkU8F32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCvtPkU8F32 not implemented");
}

void TranslationContext::vPackB32F16(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vPackB32F16 not implemented");
}

void TranslateConvertInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateConvertInstruction not implemented");
}

}
