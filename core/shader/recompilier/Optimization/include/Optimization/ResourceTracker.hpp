#ifndef SHADER_RECOMPILIER_OPTIMIZATION_RESOURCETRACKER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_RESOURCETRACKER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>

namespace ShaderRecompiler {

class ResourceTracker {
public:
    void Track(IrProgram& program) const;
};

}

#endif
