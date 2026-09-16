#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

bool TranslationContext::sLoad(const RdnaInstruction& inst, bool raw) {
    throw std::runtime_error("TranslationContext::sLoad not implemented");
}

}
