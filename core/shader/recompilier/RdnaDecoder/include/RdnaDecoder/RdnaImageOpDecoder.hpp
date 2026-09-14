#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNAIMAGEOPDECODER_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNAIMAGEOPDECODER_HPP

#include <RdnaDecoder/RdnaInstruction.hpp>
#include <cstdint>
#include <span>

namespace ShaderRecompiler {

[[nodiscard]] RdnaInstruction DecodeRdnaImageOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex);

}

#endif
