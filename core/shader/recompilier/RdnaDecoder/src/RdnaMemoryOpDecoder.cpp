#include "RdnaDecoder/RdnaMemoryOpDecoder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaMemoryOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaMemoryOp not implemented");
}

RdnaInstruction DecodeRdnaSmem(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSmem not implemented");
}

RdnaInstruction DecodeRdnaMubuf(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaMubuf not implemented");
}

RdnaInstruction DecodeRdnaMtbuf(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaMtbuf not implemented");
}

RdnaInstruction DecodeRdnaFlat(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaFlat not implemented");
}

RdnaInstruction DecodeRdnaDs(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaDs not implemented");
}

}
