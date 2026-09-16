#include "Translation/TranslationContext.hpp"
#include <array>
#include <stdexcept>

namespace ShaderRecompiler {

std::array<IrU32, 2> TranslationContext::ballotMask(IrU1 value) {
    throw std::runtime_error("TranslationContext::ballotMask not implemented");
}

IrU1 TranslationContext::threadBit(const std::array<IrU32, 2>& mask) {
    throw std::runtime_error("TranslationContext::threadBit not implemented");
}

IrU32 TranslationContext::conditionBit(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::conditionBit not implemented");
}

IrU1 TranslationContext::readMask(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readMask not implemented");
}

IrU1 TranslationContext::readMaskValid(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readMaskValid not implemented");
}

std::array<IrU32, 2> TranslationContext::writeMask(const RdnaOperand& operand, IrU1 value, bool write64) {
    throw std::runtime_error("TranslationContext::writeMask not implemented");
}

void TranslationContext::writeCompareResult(const RdnaOperand& operand, IrU1 value) {
    throw std::runtime_error("TranslationContext::writeCompareResult not implemented");
}

void TranslationContext::AddBranchCondition(const BasicBlock& source, BlockInfo& info) {
    throw std::runtime_error("TranslationContext::AddBranchCondition not implemented");
}

}
