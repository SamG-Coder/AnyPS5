#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

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
    throw std::runtime_error("DecodeRdnaScalarSource not implemented");
}

RdnaOperand DecodeRdnaScalarDestination(std::uint32_t code, std::uint32_t programCounter) {
    throw std::runtime_error("DecodeRdnaScalarDestination not implemented");
}

RdnaOperand DecodeRdnaVectorGpr(std::uint32_t reg) {
    throw std::runtime_error("DecodeRdnaVectorGpr not implemented");
}

void ReadRdnaLiteralOperands(std::span<const std::uint32_t> code, std::uint32_t wordIndex, RdnaInstruction& instruction) {
    throw std::runtime_error("ReadRdnaLiteralOperands not implemented");
}

void SetRdnaRawWords(RdnaInstruction& instruction, std::span<const std::uint32_t> code, std::uint32_t wordIndex, std::uint32_t wordCount) {
    throw std::runtime_error("SetRdnaRawWords not implemented");
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
