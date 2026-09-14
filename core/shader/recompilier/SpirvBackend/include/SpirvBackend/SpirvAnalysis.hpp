#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVANALYSIS_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVANALYSIS_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace ShaderRecompiler {

struct SpirvRequirements {
    std::vector<std::uint32_t> capabilities;
    std::vector<std::string> extensions;
};

[[nodiscard]] SpirvRequirements AnalyzeProgramRequirements(const IrProgram& program);

}

#endif
