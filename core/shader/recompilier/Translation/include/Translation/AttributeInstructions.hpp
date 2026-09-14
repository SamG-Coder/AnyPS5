#ifndef SHADER_RECOMPILIER_TRANSLATION_ATTRIBUTEINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_ATTRIBUTEINSTRUCTIONS_HPP

#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>
#include <Translation/InstructionTranslator.hpp>

namespace ShaderRecompiler {

void TranslateAttributeInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const TranslateOptions& options);

}

#endif
