#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNASCALAROPDECODER_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNASCALAROPDECODER_HPP

#include <RdnaDecoder/RdnaInstruction.hpp>
#include <cstdint>
#include <span>

namespace ShaderRecompiler {

[[nodiscard]] RdnaInstruction DecodeRdnaScalarOp(std::span<const std::uint32_t> code, std::uint32_t wordIndex);

}

#endif
