#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

bool TranslationContext::imageAtomic(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::imageAtomic not implemented");
}

bool TranslationContext::imageGetResinfo(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGetResinfo not implemented");
}

bool TranslationContext::imageGetLod(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGetLod not implemented");
}

bool TranslationContext::imageLoad(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageLoad not implemented");
}

bool TranslationContext::imageStore(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageStore not implemented");
}

bool TranslationContext::imageSample(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageSample not implemented");
}

bool TranslationContext::imageGather(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::imageGather not implemented");
}

}
