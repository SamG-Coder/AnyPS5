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

}
