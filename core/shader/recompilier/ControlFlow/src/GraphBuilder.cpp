#include <ControlFlow/GraphBuilder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

ControlFlowGraph GraphBuilder::Build(const RdnaProgram& program) const {
    throw std::runtime_error("GraphBuilder::Build not implemented");
}
std::vector<BasicBlock> GraphBuilder::splitIntoBlocks(const RdnaProgram& program) const {
    throw std::runtime_error("GraphBuilder::splitIntoBlocks not implemented");
}
void GraphBuilder::linkBlocks(std::vector<BasicBlock>& blocks, const RdnaProgram& program) const {
    throw std::runtime_error("GraphBuilder::linkBlocks not implemented");
}

}
