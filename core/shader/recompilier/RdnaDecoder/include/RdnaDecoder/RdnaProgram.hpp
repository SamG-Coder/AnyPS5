#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNAPROGRAM_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNAPROGRAM_HPP

#include <RdnaDecoder/RdnaInstruction.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct RdnaProgram {
    std::vector<RdnaInstruction> instructions;
};

[[nodiscard]] const RdnaInstruction* FindInstructionAtProgramCounter(const RdnaProgram& program, std::uint32_t programCounter);

}

#endif
