#ifndef SHADER_RECOMPILIER_TRANSLATION_INTEGERINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_INTEGERINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateIntegerInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
