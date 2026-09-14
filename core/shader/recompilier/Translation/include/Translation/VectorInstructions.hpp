#ifndef SHADER_RECOMPILIER_TRANSLATION_VECTORINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_VECTORINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateVectorInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
