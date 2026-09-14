#include <RdnaDecoder/RdnaExportOpDecoder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

namespace {

[[nodiscard]] RdnaOpcode MapExportTargetToOpcode(std::uint32_t target) {
    if (target <= 7u) {
        return RdnaOpcode::ExpMrt;
    }
    if (target >= 12u && target <= 15u) {
        return RdnaOpcode::ExpPos;
    }
    if (target >= 32u && target <= 63u) {
        return RdnaOpcode::ExpParam;
    }
    throw std::runtime_error("unsupported export target");
}

[[nodiscard]] RdnaOperand DecodeExportSourceRegister(std::uint32_t reg) {
    RdnaOperand operand{};
    operand.kind = RdnaOperandKind::VectorRegister;
    operand.reg = reg;
    operand.value = 0;
    operand.negate = false;
    operand.absolute = false;
    return operand;
}

}

RdnaInstruction DecodeRdnaExportOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    if (wordIndex + 1u >= code.size()) {
        throw std::runtime_error("truncated export instruction");
    }

    const std::uint32_t word0 = code[wordIndex];
    const std::uint32_t word1 = code[wordIndex + 1u];

    const std::uint32_t target = (word0 >> 4u) & 0x3Fu;
    const std::uint32_t en = word0 & 0xFu;
    const bool compr = ((word0 >> 10u) & 0x1u) != 0u;
    const bool done = ((word0 >> 11u) & 0x1u) != 0u;

    const RdnaOpcode op = MapExportTargetToOpcode(target);

    RdnaInstruction instruction{};
    instruction.op = op;
    instruction.programCounter = wordIndex * 4u;

    instruction.destination = RdnaOperand{};
    instruction.destination.kind = RdnaOperandKind::Null;
    instruction.destination.reg = 0;
    instruction.destination.value = 0;
    instruction.destination.negate = false;
    instruction.destination.absolute = false;

    instruction.source0 = DecodeExportSourceRegister(word1 & 0xFFu);
    instruction.source1 = DecodeExportSourceRegister((word1 >> 8u) & 0xFFu);
    instruction.source2 = DecodeExportSourceRegister((word1 >> 16u) & 0xFFu);
    instruction.source3 = DecodeExportSourceRegister((word1 >> 24u) & 0xFFu);

    instruction.branchOffset = 0;
    instruction.memoryOffset = 0;

    if (en == 0u) {
        instruction.dataDwordCount = 0u;
    } else if (compr) {
        instruction.dataDwordCount = 2u;
    } else {
        instruction.dataDwordCount = 4u;
    }

    instruction.clampResult = false;
    instruction.is64Bit = false;

    instruction.exportTarget = target;
    instruction.exportEnableMask = en;
    instruction.exportIsCompressed = compr;
    instruction.exportIsLast = done;

    return instruction;
}

}
