#include "Translation/TranslationContext.hpp"
#include <array>
#include <stdexcept>

namespace ShaderRecompiler {

IrU32 TranslationContext::readU32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU32 not implemented");
}

std::array<IrU32, 2> TranslationContext::readU32Pair(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU32Pair not implemented");
}

IrU64 TranslationContext::readU64(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU64 not implemented");
}

std::array<IrU32, 2> TranslationContext::extractU64(IrU64 value) {
    throw std::runtime_error("TranslationContext::extractU64 not implemented");
}

void TranslationContext::writeU32Pair(const RdnaOperand& operand, const std::array<IrU32, 2>& value) {
    throw std::runtime_error("TranslationContext::writeU32Pair not implemented");
}

IrF32 TranslationContext::readF16LaneAsF32(const RdnaOperand& operand, bool highLane, bool packed) {
    throw std::runtime_error("TranslationContext::readF16LaneAsF32 not implemented");
}

IrF32 TranslationContext::readF16AsF32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readF16AsF32 not implemented");
}

IrF32 TranslationContext::readMixF32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readMixF32 not implemented");
}

IrU32 TranslationContext::readF16LaneBits(const RdnaOperand& operand, bool highLane) {
    throw std::runtime_error("TranslationContext::readF16LaneBits not implemented");
}

IrU32 TranslationContext::readU16LaneRaw(const RdnaOperand& operand, bool highLane) {
    throw std::runtime_error("TranslationContext::readU16LaneRaw not implemented");
}

IrU32 TranslationContext::readU16LaneAsU32(const RdnaOperand& operand, bool highLane, bool signExtend) {
    throw std::runtime_error("TranslationContext::readU16LaneAsU32 not implemented");
}

IrU32 TranslationContext::readU16AsU32(const RdnaOperand& operand, bool signExtend) {
    throw std::runtime_error("TranslationContext::readU16AsU32 not implemented");
}

IrU32 TranslationContext::packHalf2x16(IrF32 low, IrF32 high) {
    throw std::runtime_error("TranslationContext::packHalf2x16 not implemented");
}

void TranslationContext::write16Bits(const RdnaOperand& operand, IrU32 value) {
    throw std::runtime_error("TranslationContext::write16Bits not implemented");
}

void TranslationContext::writeF16(const RdnaOperand& operand, IrF32 value) {
    throw std::runtime_error("TranslationContext::writeF16 not implemented");
}

}
