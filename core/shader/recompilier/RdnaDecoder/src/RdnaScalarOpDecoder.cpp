#include "RdnaDecoder/RdnaScalarOpDecoder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaScalarOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaScalarOp not implemented");
}

RdnaInstruction DecodeRdnaSop1(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSop1 not implemented");
}

RdnaInstruction DecodeRdnaSop2(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSop2 not implemented");
}

RdnaInstruction DecodeRdnaSopk(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSopk not implemented");
}

RdnaInstruction DecodeRdnaSopc(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSopc not implemented");
}

RdnaInstruction DecodeRdnaSopp(std::uint32_t programCounter, std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaSopp not implemented");
}

}
