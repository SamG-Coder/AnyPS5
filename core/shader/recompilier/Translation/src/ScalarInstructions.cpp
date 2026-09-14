#include "Translation/ScalarInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateScalarInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateScalarInstruction not implemented");
}

bool TranslationContext::emitScalar(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::emitScalar not implemented");
}

void TranslateScalarInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateScalarInstruction not implemented");
}

}
