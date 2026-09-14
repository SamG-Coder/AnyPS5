#ifndef SHADER_RECOMPILIER_TRANSLATION_DISPATCHINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_DISPATCHINSTRUCTIONS_HPP

#include <ControlFlow/ControlFlowGraph.hpp>
#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>
#include <Translation/InstructionTranslator.hpp>

namespace ShaderRecompiler {

void DispatchInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg, const TranslateOptions& options);

}

#endif
