#ifndef SHADER_RECOMPILIER_OPTIMIZATION_READLANEELIMINATOR_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_READLANEELIMINATOR_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <cstdint>

namespace ShaderRecompiler {

struct ReadLaneEliminationStats {
    std::uint32_t rewrittenReads;
};

class ReadLaneEliminator {
public:
    [[nodiscard]] ReadLaneEliminationStats Eliminate(IrProgram& program, std::uint32_t waveSize) const;
};

}

#endif
