#include "Translation/MemoryInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrValue* TranslationContext::loadSharedU32(std::uint32_t width, IrU32 address, const MemoryInfo& memory, std::uint32_t pc) {
    throw std::runtime_error("TranslationContext::loadSharedU32 not implemented");
}

IrValue* TranslationContext::extractSharedU32(IrValue* value, std::uint32_t width, std::uint32_t index) {
    throw std::runtime_error("TranslationContext::extractSharedU32 not implemented");
}

void TranslationContext::writeSharedU32(std::uint32_t width, IrU32 address, const std::array<IrValue*, 4>& values, const MemoryInfo& memory, std::uint32_t pc) {
    throw std::runtime_error("TranslationContext::writeSharedU32 not implemented");
}

bool TranslationContext::dsRead(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsRead not implemented");
}

bool TranslationContext::dsRead2(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsRead2 not implemented");
}

bool TranslationContext::dsWrite(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsWrite not implemented");
}

bool TranslationContext::dsWrite2(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsWrite2 not implemented");
}

bool TranslationContext::dsMinmaxF32(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::dsMinmaxF32 not implemented");
}

bool TranslationContext::dsAppendConsume(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::dsAppendConsume not implemented");
}

bool TranslationContext::dsAddtid(const RdnaInstruction& inst, bool write) {
    throw std::runtime_error("TranslationContext::dsAddtid not implemented");
}

bool TranslationContext::dsSwizzleB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsSwizzleB32 not implemented");
}

bool TranslationContext::dsBpermuteB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::dsBpermuteB32 not implemented");
}

bool TranslationContext::dsAtomic(const RdnaInstruction& inst, IrOpcode opcode, bool returnsValue) {
    throw std::runtime_error("TranslationContext::dsAtomic not implemented");
}

}
