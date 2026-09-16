#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

bool TranslationContext::flatLoad(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::flatLoad not implemented");
}

bool TranslationContext::flatStore(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::flatStore not implemented");
}

}
