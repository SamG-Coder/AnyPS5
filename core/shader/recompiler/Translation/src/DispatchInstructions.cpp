#include "Translation/DispatchInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void DispatchInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg, const TranslateOptions& options) {
    throw std::runtime_error("DispatchInstruction not implemented");
}

void TranslationContext::TranslateInstruction(const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslationContext::TranslateInstruction not implemented");
}

void DispatchInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("DispatchInstruction not implemented");
}

}
