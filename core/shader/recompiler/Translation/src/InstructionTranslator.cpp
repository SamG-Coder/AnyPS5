#include "Translation/InstructionTranslator.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrProgram InstructionTranslator::Translate(const RdnaProgram& decoded, const ControlFlowGraph& cfg, const TranslateOptions& options) const {
    throw std::runtime_error("InstructionTranslator::Translate not implemented");
}
void InstructionTranslator::translateInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg, const TranslateOptions& options) const {
    throw std::runtime_error("InstructionTranslator::translateInstruction not implemented");
}

TranslationContext::TranslationContext(IrProgram& program, IrBlock& block, std::uint32_t vectorLimit) : program(program), ir(program), block(block), currentVectorLimit(vectorLimit) {
    throw std::runtime_error("TranslationContext not implemented");
}

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

std::array<IrU32, 2> TranslationContext::ballotMask(IrU1 value) {
    throw std::runtime_error("TranslationContext::ballotMask not implemented");
}

IrU32 TranslationContext::readRawU32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readRawU32 not implemented");
}

IrU32 TranslationContext::readScalarCode(std::uint32_t code) {
    throw std::runtime_error("TranslationContext::readScalarCode not implemented");
}

IrU32 TranslationContext::applyBitSourceModifiers(const RdnaOperand& operand, IrU32 value) {
    throw std::runtime_error("TranslationContext::applyBitSourceModifiers not implemented");
}

IrValue* TranslationContext::readOperand(const RdnaOperand& operand, IrType type) {
    throw std::runtime_error("TranslationContext::readOperand not implemented");
}

IrU1 TranslationContext::threadBit(const std::array<IrU32, 2>& mask) {
    throw std::runtime_error("TranslationContext::threadBit not implemented");
}

void TranslationContext::writeRawU32(const RdnaOperand& operand, IrU32 value) {
    throw std::runtime_error("TranslationContext::writeRawU32 not implemented");
}

IrF32 TranslationContext::applyF32ResultModifiers(const RdnaOperand& operand, IrF32 value) {
    throw std::runtime_error("TranslationContext::applyF32ResultModifiers not implemented");
}

void TranslationContext::writeOperand(const RdnaOperand& operand, IrValue* value) {
    throw std::runtime_error("TranslationContext::writeOperand not implemented");
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

IrU32 TranslationContext::readU32(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU32 not implemented");
}

std::array<IrU32, 2> TranslationContext::readU32Pair(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU32Pair not implemented");
}

IrU64 TranslationContext::readU64(const RdnaOperand& operand) {
    throw std::runtime_error("TranslationContext::readU64 not implemented");
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

IrU32 TranslationContext::readU16LaneRaw(const RdnaOperand& operand, bool highLane) {
    throw std::runtime_error("TranslationContext::readU16LaneRaw not implemented");
}

IrU32 TranslationContext::readU16LaneAsU32(const RdnaOperand& operand, bool highLane, bool signExtend) {
    throw std::runtime_error("TranslationContext::readU16LaneAsU32 not implemented");
}

IrU32 TranslationContext::readU16AsU32(const RdnaOperand& operand, bool signExtend) {
    throw std::runtime_error("TranslationContext::readU16AsU32 not implemented");
}

IrU32 TranslationContext::readF16LaneBits(const RdnaOperand& operand, bool highLane) {
    throw std::runtime_error("TranslationContext::readF16LaneBits not implemented");
}

std::array<IrU32, 2> TranslationContext::extractU64(IrU64 value) {
    throw std::runtime_error("TranslationContext::extractU64 not implemented");
}

void TranslationContext::writeU32Pair(const RdnaOperand& operand, const std::array<IrU32, 2>& value) {
    throw std::runtime_error("TranslationContext::writeU32Pair not implemented");
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
