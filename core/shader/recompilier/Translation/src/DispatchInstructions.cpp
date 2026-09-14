#include <Translation/DispatchInstructions.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void DispatchInstruction(IrBuilder& builder, const RdnaInstruction& instruction, const ControlFlowGraph& cfg, const TranslateOptions& options) {
    throw std::runtime_error("DispatchInstruction not implemented");
}

}
