#ifndef SHADER_RECOMPILIER_TRANSLATION_COMPAREINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_COMPAREINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateCompareInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
