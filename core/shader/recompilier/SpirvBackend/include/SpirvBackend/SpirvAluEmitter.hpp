#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVALUEMITTER_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVALUEMITTER_HPP

#include <IntermediateRepresentation/IrValue.hpp>
#include <SpirvBackend/SpirvModule.hpp>
#include <cstdint>

namespace ShaderRecompiler {

void EmitAluValue(SpirvModule& module, const IrValue& value, std::uint32_t resultId);

}

#endif
