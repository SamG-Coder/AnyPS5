#include <ControlFlow/Structurizer.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void Structurizer::Structurize(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::Structurize not implemented");
}
void Structurizer::computeDominatorTree(const ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::computeDominatorTree not implemented");
}
void Structurizer::detectNaturalLoops(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::detectNaturalLoops not implemented");
}
void Structurizer::verifyReducibility(const ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::verifyReducibility not implemented");
}

}
