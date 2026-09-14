#ifndef SHADER_RECOMPILIER_OPTIMIZATION_DEADCODEELIMINATOR_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_DEADCODEELIMINATOR_HPP

#include <IntermediateRepresentation/IrProgram.hpp>

namespace ShaderRecompiler {

class DeadCodeEliminator {
public:
    void Eliminate(IrProgram& program) const;
};

}

#endif
