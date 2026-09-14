#ifndef SHADER_RECOMPILIER_OPTIMIZATION_SRTWALKER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_SRTWALKER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>

namespace ShaderRecompiler {

class SrtWalker {
public:
    void BuildPlan(IrProgram& program) const;
};

}

#endif
