#include "RdnaDecoder/RdnaVectorOpDecoder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaVectorOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVectorOp not implemented");
}

RdnaInstruction DecodeRdnaVop1(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVop1 not implemented");
}

RdnaInstruction DecodeRdnaVop2(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVop2 not implemented");
}

RdnaInstruction DecodeRdnaVop3(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVop3 not implemented");
}

RdnaInstruction DecodeRdnaVop3p(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVop3p not implemented");
}

RdnaInstruction DecodeRdnaVopc(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVopc not implemented");
}

RdnaInstruction DecodeRdnaVintrp(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaVintrp not implemented");
}

}
