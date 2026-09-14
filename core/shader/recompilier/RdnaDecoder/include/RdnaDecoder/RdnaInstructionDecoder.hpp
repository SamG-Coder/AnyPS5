#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNAINSTRUCTIONDECODER_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNAINSTRUCTIONDECODER_HPP

#include <RdnaDecoder/RdnaProgram.hpp>
#include <cstdint>
#include <span>

namespace ShaderRecompiler {

class RdnaInstructionDecoder {
public:
    [[nodiscard]] RdnaProgram Decode(std::span<const std::uint32_t> code) const;

private:
    [[nodiscard]] RdnaInstruction decodeAt(std::span<const std::uint32_t> code, std::uint32_t wordIndex) const;
};

}

#endif
