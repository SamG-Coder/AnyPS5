#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateMemoryInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateMemoryInstruction not implemented");
}

bool TranslationContext::emitMemory(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::emitMemory not implemented");
}

void TranslateMemoryInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateMemoryInstruction not implemented");
}

}
