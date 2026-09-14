#ifndef SHADER_RECOMPILIER_OPTIMIZATION_SHADERINFOCOLLECTOR_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_SHADERINFOCOLLECTOR_HPP

#include <IntermediateRepresentation/IrProgram.hpp>

namespace ShaderRecompiler {

class ShaderInfoCollector {
public:
    void Collect(IrProgram& program) const;
};

}

#endif
