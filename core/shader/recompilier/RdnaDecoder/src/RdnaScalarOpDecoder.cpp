#include <RdnaDecoder/RdnaScalarOpDecoder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaScalarOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaScalarOp not implemented");
}

}
