#ifndef SHADER_RECOMPILIER_TRANSLATION_CONTROLFLOWINSTRUCTIONS_HPP
#define SHADER_RECOMPILIER_TRANSLATION_CONTROLFLOWINSTRUCTIONS_HPP

#include <ControlFlow/ControlFlowGraph.hpp>
#include <IntermediateRepresentation/IrBuilder.hpp>
#include <RdnaDecoder/RdnaInstruction.hpp>

namespace ShaderRecompiler {

void TranslateControlFlowInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg);

}

#endif
