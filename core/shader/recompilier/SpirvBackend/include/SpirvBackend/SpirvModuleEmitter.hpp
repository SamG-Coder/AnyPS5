#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVMODULEEMITTER_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVMODULEEMITTER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <Optimization/BindingAllocator.hpp>
#include <SpirvBackend/SpirvModule.hpp>

namespace ShaderRecompiler {

void EmitModuleHeader(SpirvModule& module, const IrProgram& program, const BindingAllocationResult& bindings);

}

#endif
