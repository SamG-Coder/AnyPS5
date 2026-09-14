#include <RdnaDecoder/RdnaProgram.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

const RdnaInstruction* FindInstructionAtProgramCounter(const RdnaProgram& program, std::uint32_t programCounter) {
    throw std::runtime_error("FindInstructionAtProgramCounter not implemented");
}

}
