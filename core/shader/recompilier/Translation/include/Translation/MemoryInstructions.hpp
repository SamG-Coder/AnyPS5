#ifndef SHADER_RECOMPILIER_TRANSLATION_MEMORYINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_MEMORYINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateMemoryInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
