#ifndef SHADER_RECOMPILIER_OPTIMIZATION_SSABUILDER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_SSABUILDER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>

namespace ShaderRecompiler {

class SsaBuilder {
public:
    void Rewrite(IrProgram& program) const;

private:
    void insertPhiNodes(IrProgram& program) const;
    void renameVariables(IrProgram& program) const;
};

}

#endif
