#ifndef CORE_SHADER_RECOMPILER_OPTIMIZATION_RESOURCEPROGRAM_HPP
#define CORE_SHADER_RECOMPILER_OPTIMIZATION_RESOURCEPROGRAM_HPP

#include "Recompiler.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"

namespace ShaderRecompiler {

[[nodiscard]] IrProgram PrepareResourceProgram(const RecompileRequest& request);

}

#endif
