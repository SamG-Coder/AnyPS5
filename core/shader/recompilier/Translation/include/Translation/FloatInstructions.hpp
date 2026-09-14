#ifndef SHADER_RECOMPILIER_TRANSLATION_FLOATINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_FLOATINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

[[nodiscard]] void TranslateFloatInstruction(IrBuilder& builder, const RdnaInstruction& instruction);

}

#endif
