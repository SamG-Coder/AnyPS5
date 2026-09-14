#include <RdnaDecoder/RdnaExportOpDecoder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

RdnaInstruction DecodeRdnaExportOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex) {
    throw std::runtime_error("DecodeRdnaExportOp not implemented");
}

}
