#include "Translation/IntegerInstructions.hpp"
#include "Translation/TranslationContext.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void TranslateIntegerInstruction(IrBuilder& builder, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateIntegerInstruction not implemented");
}

bool TranslationContext::integer16Shift(const RdnaInstruction& inst, IrOpcode opcode, bool arithmetic) {
    throw std::runtime_error("TranslationContext::integer16Shift not implemented");
}

bool TranslationContext::integer16Binary(const RdnaInstruction& inst, IrOpcode opcode, bool sign) {
    throw std::runtime_error("TranslationContext::integer16Binary not implemented");
}

bool TranslationContext::vMed3I16(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vMed3I16 not implemented");
}

bool TranslationContext::packedInteger16Shift(const RdnaInstruction& inst, IrOpcode opcode, bool arithmetic) {
    throw std::runtime_error("TranslationContext::packedInteger16Shift not implemented");
}

bool TranslationContext::packedInteger16Binary(const RdnaInstruction& inst, IrOpcode opcode) {
    throw std::runtime_error("TranslationContext::packedInteger16Binary not implemented");
}

bool TranslationContext::packedInteger16Mad(const RdnaInstruction& inst, bool sign) {
    throw std::runtime_error("TranslationContext::packedInteger16Mad not implemented");
}

bool TranslationContext::packedInteger16MinMax(const RdnaInstruction& inst, IrOpcode opcode, bool sign) {
    throw std::runtime_error("TranslationContext::packedInteger16MinMax not implemented");
}

bool TranslationContext::sU64Mask(const RdnaInstruction& inst, IrOpcode logicalOpcode, IrOpcode bitOpcode, bool negateRhs, bool negateResult, bool unary) {
    throw std::runtime_error("TranslationContext::sU64Mask not implemented");
}

IrU1 TranslationContext::u64MaskBinary(const RdnaInstruction& inst, IrOpcode opcode, bool negateRhs, bool negateResult) {
    throw std::runtime_error("TranslationContext::u64MaskBinary not implemented");
}

bool TranslationContext::simpleInteger(const RdnaInstruction& inst, IrOpcode opcode, IrType type, bool reverse, bool maskShiftCount, bool updateScc) {
    throw std::runtime_error("TranslationContext::simpleInteger not implemented");
}

bool TranslationContext::composedIntegerBinary(const RdnaInstruction& inst, IrOpcode opcode, bool negateRhs, bool negateResult, bool updateScc) {
    throw std::runtime_error("TranslationContext::composedIntegerBinary not implemented");
}

bool TranslationContext::vAndOrB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vAndOrB32 not implemented");
}

bool TranslationContext::vOr3B32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vOr3B32 not implemented");
}

bool TranslationContext::vXor3B32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vXor3B32 not implemented");
}

bool TranslationContext::sFf1I32B64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sFf1I32B64 not implemented");
}

bool TranslationContext::vFfbh32(const RdnaInstruction& inst, bool sign) {
    throw std::runtime_error("TranslationContext::vFfbh32 not implemented");
}

bool TranslationContext::sFlbitI32B64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sFlbitI32B64 not implemented");
}

bool TranslationContext::integer24(const RdnaInstruction& inst, bool sign, bool addend) {
    throw std::runtime_error("TranslationContext::integer24 not implemented");
}

bool TranslationContext::vMadU64U32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vMadU64U32 not implemented");
}

bool TranslationContext::vSadU32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vSadU32 not implemented");
}

bool TranslationContext::vAdd3U32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vAdd3U32 not implemented");
}

bool TranslationContext::sBitsetB32(const RdnaInstruction& inst, bool set) {
    throw std::runtime_error("TranslationContext::sBitsetB32 not implemented");
}

bool TranslationContext::sBitsetB64(const RdnaInstruction& inst, bool set) {
    throw std::runtime_error("TranslationContext::sBitsetB64 not implemented");
}

bool TranslationContext::vBcntU32B32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vBcntU32B32 not implemented");
}

bool TranslationContext::vMbcntU32B32(const RdnaInstruction& inst, bool low) {
    throw std::runtime_error("TranslationContext::vMbcntU32B32 not implemented");
}

bool TranslationContext::sBitreplicateB64B32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sBitreplicateB64B32 not implemented");
}

bool TranslationContext::sQuadmaskB64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sQuadmaskB64 not implemented");
}

bool TranslationContext::bfmB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::bfmB32 not implemented");
}

IrU32 TranslationContext::rightMask32(IrU32 count) {
    throw std::runtime_error("TranslationContext::rightMask32 not implemented");
}

IrU64 TranslationContext::rightMask64(IrU32 count) {
    throw std::runtime_error("TranslationContext::rightMask64 not implemented");
}

bool TranslationContext::sBfmB64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sBfmB64 not implemented");
}

bool TranslationContext::sBfeU32(const RdnaInstruction& inst, bool sign) {
    throw std::runtime_error("TranslationContext::sBfeU32 not implemented");
}

bool TranslationContext::sBfeU64(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::sBfeU64 not implemented");
}

bool TranslationContext::vBfeU32(const RdnaInstruction& inst, bool sign) {
    throw std::runtime_error("TranslationContext::vBfeU32 not implemented");
}

bool TranslationContext::vBfiB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vBfiB32 not implemented");
}

bool TranslationContext::sBitcmpB32(const RdnaInstruction& inst, bool expected) {
    throw std::runtime_error("TranslationContext::sBitcmpB32 not implemented");
}

bool TranslationContext::vAlignbitB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vAlignbitB32 not implemented");
}

bool TranslationContext::vAlignbyteB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vAlignbyteB32 not implemented");
}

bool TranslationContext::vLshlAddU32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vLshlAddU32 not implemented");
}

bool TranslationContext::vAddLshlU32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vAddLshlU32 not implemented");
}

bool TranslationContext::vXadU32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vXadU32 not implemented");
}

bool TranslationContext::vLshlOrB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vLshlOrB32 not implemented");
}

bool TranslationContext::vCndmaskB32(const RdnaInstruction& inst) {
    throw std::runtime_error("TranslationContext::vCndmaskB32 not implemented");
}

bool TranslationContext::packB16(const RdnaInstruction& inst, bool high0, bool high1) {
    throw std::runtime_error("TranslationContext::packB16 not implemented");
}

void TranslateIntegerInstruction(TranslationContext& context, const RdnaInstruction& instruction) {
    throw std::runtime_error("TranslateIntegerInstruction not implemented");
}

}
