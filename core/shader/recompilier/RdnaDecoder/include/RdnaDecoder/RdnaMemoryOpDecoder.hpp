#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNAMEMORYOPDECODER_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNAMEMORYOPDECODER_HPP

#include <RdnaDecoder/RdnaInstruction.hpp>
#include <cstdint>
#include <span>

namespace ShaderRecompiler {

[[nodiscard]] RdnaInstruction DecodeRdnaMemoryOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex);

}

#endif
