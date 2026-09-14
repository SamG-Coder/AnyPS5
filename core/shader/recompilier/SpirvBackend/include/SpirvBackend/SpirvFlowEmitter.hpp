#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVFLOWEMITTER_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVFLOWEMITTER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <SpirvBackend/SpirvModule.hpp>

namespace ShaderRecompiler {

void EmitControlFlow(SpirvModule& module, const IrProgram& program);

}

#endif
