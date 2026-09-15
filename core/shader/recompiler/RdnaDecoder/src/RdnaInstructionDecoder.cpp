#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include <bit>
#include <stdexcept>
#include <string>

namespace ShaderRecompiler {

namespace {

bool instructionHasLiteral(const RdnaInstruction& instruction) {
    return instruction.source0.kind == RdnaOperandKind::LiteralConstant ||
        instruction.source1.kind == RdnaOperandKind::LiteralConstant ||
        instruction.source2.kind == RdnaOperandKind::LiteralConstant ||
        instruction.source3.kind == RdnaOperandKind::LiteralConstant;
}

void applyLiteral(RdnaOperand& operand, std::uint32_t literal) {
    if (operand.kind != RdnaOperandKind::LiteralConstant) {
        return;
    }

    operand.value = literal;
    operand.signedVal = static_cast<std::int32_t>(literal);
}

}

RdnaProgram RdnaInstructionDecoder::Decode(std::span<const std::uint32_t> code) const {
    throw std::runtime_error("RdnaInstructionDecoder::Decode not implemented");
}

RdnaInstruction RdnaInstructionDecoder::decodeAt(std::span<const std::uint32_t> code, std::uint32_t wordIndex) const {
    throw std::runtime_error("RdnaInstructionDecoder::decodeAt not implemented");
}

RdnaInstructionFamily GetRdnaInstructionFamily(std::uint32_t word) {
    throw std::runtime_error("GetRdnaInstructionFamily not implemented");
}

RdnaInstruction DecodeRdnaInstruction(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaInstruction not implemented");
}

RdnaProgram DecodeRdnaFrontProgram(std::span<const std::uint32_t> front) {
    throw std::runtime_error("DecodeRdnaFrontProgram not implemented");
}

void DecodeRdnaProgram(std::span<const std::uint32_t> code, RdnaProgram& program) {
    throw std::runtime_error("DecodeRdnaProgram not implemented");
}

RdnaOperand DecodeRdnaScalarSource(std::uint32_t code, std::uint32_t programCounter) {
    RdnaOperand operand;

    if (code <= 105u) {
        operand.kind = RdnaOperandKind::ScalarRegister;
        operand.reg = code;
        return operand;
    }

    if (code >= 128u && code <= 192u) {
        operand.kind = RdnaOperandKind::IntegerInlineConstant;
        operand.signedVal = static_cast<std::int32_t>(code - 128u);
        operand.value = static_cast<std::uint32_t>(operand.signedVal);
        return operand;
    }

    if (code >= 193u && code <= 208u) {
        operand.kind = RdnaOperandKind::IntegerInlineConstant;
        operand.signedVal = 192 - static_cast<std::int32_t>(code);
        operand.value = static_cast<std::uint32_t>(operand.signedVal);
        return operand;
    }

    if (code >= 240u && code <= 247u) {
        static constexpr float inlineFloatConstants[] = {0.5f, -0.5f, 1.0f, -1.0f, 2.0f, -2.0f, 4.0f, -4.0f};
        operand.kind = RdnaOperandKind::FloatInlineConstant;
        operand.value = std::bit_cast<std::uint32_t>(inlineFloatConstants[code - 240u]);
        return operand;
    }

    if (code >= 256u && code <= 511u) {
        return DecodeRdnaVectorGpr(code - 256u);
    }

    switch (code) {
        case 106u: operand.kind = RdnaOperandKind::VccLo; return operand;
        case 107u: operand.kind = RdnaOperandKind::VccHi; return operand;
        case 124u: operand.kind = RdnaOperandKind::M0; return operand;
        case 125u: operand.kind = RdnaOperandKind::Null; return operand;
        case 126u: operand.kind = RdnaOperandKind::ExecLo; return operand;
        case 127u: operand.kind = RdnaOperandKind::ExecHi; return operand;
        case 239u: operand.kind = RdnaOperandKind::PopsExitingWaveId; return operand;
        case 248u:
            operand.kind = RdnaOperandKind::FloatInlineConstant;
            operand.value = std::bit_cast<std::uint32_t>(0.15915494309189535f);
            return operand;
        case 251u: operand.kind = RdnaOperandKind::VccZ; return operand;
        case 252u: operand.kind = RdnaOperandKind::ExecZ; return operand;
        case 253u: operand.kind = RdnaOperandKind::Scc; return operand;
        case 255u: operand.kind = RdnaOperandKind::LiteralConstant; return operand;
        default: break;
    }

    throw std::invalid_argument("unsupported scalar source operand code " + std::to_string(code) + " at program counter " + std::to_string(programCounter));
}

RdnaOperand DecodeRdnaScalarDestination(std::uint32_t code, std::uint32_t programCounter) {
    RdnaOperand operand;

    if (code <= 105u) {
        operand.kind = RdnaOperandKind::ScalarRegister;
        operand.reg = code;
        return operand;
    }

    switch (code) {
        case 106u: operand.kind = RdnaOperandKind::VccLo; return operand;
        case 107u: operand.kind = RdnaOperandKind::VccHi; return operand;
        case 124u: operand.kind = RdnaOperandKind::M0; return operand;
        case 125u: operand.kind = RdnaOperandKind::Null; return operand;
        case 126u: operand.kind = RdnaOperandKind::ExecLo; return operand;
        case 127u: operand.kind = RdnaOperandKind::ExecHi; return operand;
        default: break;
    }

    throw std::invalid_argument("unsupported scalar destination operand code " + std::to_string(code) + " at program counter " + std::to_string(programCounter));
}

RdnaOperand DecodeRdnaVectorGpr(std::uint32_t reg) {
    RdnaOperand operand;
    operand.kind = RdnaOperandKind::VectorRegister;
    operand.reg = reg;
    return operand;
}

void SetRdnaRawWords(RdnaInstruction& instruction, std::span<const std::uint32_t> code, std::uint32_t wordIndex, std::uint32_t wordCount) {
    if (wordCount > MaxRdnaInstructionRawWords) {
        throw std::invalid_argument("raw word count exceeds the maximum instruction word capacity");
    }
    if (static_cast<std::size_t>(wordIndex) + wordCount > code.size()) {
        throw std::out_of_range("instruction words exceed the bounds of the code span");
    }

    instruction.wordCount = wordCount;
    for (std::uint32_t i = 0; i < wordCount; ++i) {
        instruction.rawWords[i] = code[wordIndex + i];
    }
}

void ReadRdnaLiteralOperands(std::span<const std::uint32_t> code, std::uint32_t wordIndex, RdnaInstruction& instruction) {
    if (!instructionHasLiteral(instruction)) {
        return;
    }
    if (static_cast<std::size_t>(wordIndex) + instruction.wordCount >= code.size()) {
        throw std::out_of_range("literal operand word is out of the code span bounds");
    }

    const std::uint32_t literal = code[wordIndex + instruction.wordCount];
    applyLiteral(instruction.source0, literal);
    applyLiteral(instruction.source1, literal);
    applyLiteral(instruction.source2, literal);
    applyLiteral(instruction.source3, literal);
    instruction.wordCount++;
    SetRdnaRawWords(instruction, code, wordIndex, instruction.wordCount);
}

void SetRdnaUnsupported(RdnaInstruction& instruction, RdnaInstructionFamily family, std::uint32_t opcodeId, const char* reason) {
    throw std::runtime_error("SetRdnaUnsupported not implemented");
}

std::string RdnaOperandToString(const RdnaOperand& operand) {
    throw std::runtime_error("RdnaOperandToString not implemented");
}

const char* RdnaImageDimensionToString(RdnaImageDimension dimension) {
    throw std::runtime_error("RdnaImageDimensionToString not implemented");
}

std::string RdnaInstructionToString(const RdnaInstruction& instruction) {
    throw std::runtime_error("RdnaInstructionToString not implemented");
}

std::string RdnaProgramToString(const RdnaProgram& program) {
    throw std::runtime_error("RdnaProgramToString not implemented");
}

}
