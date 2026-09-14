#ifndef SHADER_RECOMPILIER_TRANSLATION_CONVERTINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_CONVERTINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateConvertInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
