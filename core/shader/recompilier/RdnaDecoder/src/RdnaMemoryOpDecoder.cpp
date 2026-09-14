#include <RdnaDecoder/RdnaMemoryOpDecoder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaMemoryOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaMemoryOp not implemented");
}

}
