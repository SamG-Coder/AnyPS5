#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVIMAGEEMITTER_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVIMAGEEMITTER_HPP

#include <IntermediateRepresentation/IrValue.hpp>
#include <SpirvBackend/SpirvModule.hpp>
#include <cstdint>

namespace ShaderRecompiler {

void EmitImageOperation(SpirvModule& module, const IrValue& value, std::uint32_t resultId);

}

#endif
