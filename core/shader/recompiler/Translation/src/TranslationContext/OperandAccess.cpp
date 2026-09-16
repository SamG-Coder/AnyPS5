#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

const RdnaOperand& TranslationContext::sourceAt(const RdnaInstruction& inst, std::uint32_t index) {
    throw std::runtime_error("TranslationContext::sourceAt not implemented");
}

RdnaOperand TranslationContext::destinationOperand(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::destinationOperand not implemented");
}

RdnaOperand TranslationContext::offsetOperand(const RdnaOperand& operand, std::uint32_t offset) {
    throw std::runtime_error("TranslationContext::offsetOperand not implemented");
}

RdnaOperand TranslationContext::scalarDestinationOperand(const RdnaOperand& operand, std::uint32_t offset) {
    throw std::runtime_error("TranslationContext::scalarDestinationOperand not implemented");
}

RdnaOperand TranslationContext::plainOperand(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::plainOperand not implemented");
}

IrValue* TranslationContext::readOperand(const RdnaOperand& operand, IrType type) {
    throw std::runtime_error("TranslationContext::readOperand not implemented");
}

void TranslationContext::writeOperand(const RdnaOperand& operand, IrValue* value) {
    throw std::runtime_error("TranslationContext::writeOperand not implemented");
}

IrU32 TranslationContext::applyBitSourceModifiers(const RdnaOperand& operand, IrU32 value) {
    throw std::runtime_error("TranslationContext::applyBitSourceModifiers not implemented");
}

IrF32 TranslationContext::applyF32ResultModifiers(const RdnaOperand& operand, IrF32 value) {
    throw std::runtime_error("TranslationContext::applyF32ResultModifiers not implemented");
}

IrU32 TranslationContext::readScalarCode(std::uint32_t code) {
    throw std::runtime_error("TranslationContext::readScalarCode not implemented");
}

IrU32 TranslationContext::readRawU32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readRawU32 not implemented");
}

void TranslationContext::writeRawU32(const RdnaOperand& operand, IrU32 value) {
    throw std::runtime_error("TranslationContext::writeRawU32 not implemented");
}

}
