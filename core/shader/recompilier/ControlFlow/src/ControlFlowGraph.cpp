#include <ControlFlow/ControlFlowGraph.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

const BasicBlock& ControlFlowGraph::FindBlock(std::uint32_t blockId) const {
    throw std::runtime_error("ControlFlowGraph::FindBlock not implemented");
}

}
