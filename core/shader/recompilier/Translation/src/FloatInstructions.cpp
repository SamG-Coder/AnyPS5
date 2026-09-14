#include "Translation/FloatInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateFloatInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateFloatInstruction not implemented");
}

bool TranslationContext::packedFloat16(const RdnaInstruction& inst, IrOpcode opcode, bool accumulator, bool quietSnan) {
    throw std::runtime_error("TranslationContext::packedFloat16 not implemented");
}

bool TranslationContext::float16Unary(const RdnaInstruction& inst, IrOpcode opcode, bool invalidNegative) {
    throw std::runtime_error("TranslationContext::float16Unary not implemented");
}

bool TranslationContext::float16Trig(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::float16Trig not implemented");
}

bool TranslationContext::float16Binary(const RdnaInstruction& inst, IrOpcode opcode, bool reverse) {
    throw std::runtime_error("TranslationContext::float16Binary not implemented");
}

bool TranslationContext::float16Ternary(const RdnaInstruction& inst, IrOpcode opcode, bool accumulator, bool mix) {
    throw std::runtime_error("TranslationContext::float16Ternary not implemented");
}

bool TranslationContext::floatUnary(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::floatUnary not implemented");
}

bool TranslationContext::floatBinary(const RdnaInstruction& inst, IrOpcode opcode, bool reverse) {
    throw std::runtime_error("TranslationContext::floatBinary not implemented");
}

bool TranslationContext::floatTernary(const RdnaInstruction& inst, IrOpcode opcode, bool accumulator, bool mix) {
    throw std::runtime_error("TranslationContext::floatTernary not implemented");
}

bool TranslationContext::vFrexpMantF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vFrexpMantF32 not implemented");
}

bool TranslationContext::vDot2cF32F16(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vDot2cF32F16 not implemented");
}

bool TranslationContext::vCubeidF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCubeidF32 not implemented");
}

bool TranslationContext::vCubescF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCubescF32 not implemented");
}

bool TranslationContext::vCubetcF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCubetcF32 not implemented");
}

bool TranslationContext::vCubemaF32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCubemaF32 not implemented");
}

bool TranslationContext::floatCube(const RdnaInstruction& inst, std::uint32_t resultKind) {
    throw std::runtime_error("TranslationContext::floatCube not implemented");
}

void TranslateFloatInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateFloatInstruction not implemented");
}

}
