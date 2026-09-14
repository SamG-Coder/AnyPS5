#include <RdnaDecoder/RdnaInstructionDecoder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

RdnaProgram RdnaInstructionDecoder::Decode(std::span<const std::uint32_t> code) const {
    throw std::runtime_error("RdnaInstructionDecoder::Decode not implemented");
}
RdnaInstruction RdnaInstructionDecoder::decodeAt(std::span<const std::uint32_t> code, std::uint32_t wordIndex) const {
    throw std::runtime_error("RdnaInstructionDecoder::decodeAt not implemented");
}

}
