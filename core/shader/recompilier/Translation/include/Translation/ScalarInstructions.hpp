#ifndef SHADER_RECOMPILIER_TRANSLATION_SCALARINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_SCALARINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateScalarInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
