#include "Translation/VectorInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateVectorInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateVectorInstruction not implemented");
}

bool TranslationContext::emitVector(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::emitVector not implemented");
}

void TranslateVectorInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateVectorInstruction not implemented");
}

}
