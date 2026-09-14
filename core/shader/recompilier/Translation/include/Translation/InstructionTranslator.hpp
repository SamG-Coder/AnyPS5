#ifndef SHADER_RECOMPILIER_TRANSLATION_INSTRUCTIONTRANSLATOR_HPP
#define SHADER_RECOMPILIER_TRANSLATION_INSTRUCTIONTRANSLATOR_HPP

#include <ControlFlow/ControlFlowGraph.hpp>
#include <RdnaDecoder/RdnaProgram.hpp>
#include <IntermediateRepresentation/IrBuilder.hpp>
#include <IntermediateRepresentation/IrProgram.hpp>
#include <Translation/EmbeddedVertexFetch.hpp>
#include <cstdint>

namespace ShaderRecompiler {

enum class ShaderStageKind {
    Compute,
    Vertex,
    Pixel
};

struct TranslateOptions {
    ShaderStageKind stage;
    std::uint32_t waveSize;
    std::uint32_t userDataBaseRegister;
    std::uint32_t userDataCount;
    std::uint32_t scratchDwords;
    const EmbeddedFetchPlan* embeddedFetch;
};

class InstructionTranslator {
public:
    [[nodiscard]] IrProgram Translate(const RdnaProgram& decoded, const ControlFlowGraph& cfg, const TranslateOptions& options) const;

private:
    void translateInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg, const TranslateOptions& options) const;
};

}

#endif
