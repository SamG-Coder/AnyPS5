#include "ControlFlow/Structurizer.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void Structurizer::Structurize(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::Structurize not implemented");
}
void Structurizer::computeDominatorTree(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::computeDominatorTree not implemented");
}
void Structurizer::detectNaturalLoops(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::detectNaturalLoops not implemented");
}
void Structurizer::verifyReducibility(const ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::verifyReducibility not implemented");
}

void Structurizer::computePostDominators(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::computePostDominators not implemented");
}

void Structurizer::computeBackEdges(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::computeBackEdges not implemented");
}

void Structurizer::computeStronglyConnectedComponents(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::computeStronglyConnectedComponents not implemented");
}

void Structurizer::recomputeAnalyses(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::recomputeAnalyses not implemented");
}

void Structurizer::canonicalizeNaturalLoops(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::canonicalizeNaturalLoops not implemented");
}

void Structurizer::splitSharedMergeBlocks(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::splitSharedMergeBlocks not implemented");
}

void Structurizer::isolateSemanticLoopHeaders(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::isolateSemanticLoopHeaders not implemented");
}

void Structurizer::clearStructuredTerminators(ControlFlowGraph& graph) const {
    throw std::runtime_error("Structurizer::clearStructuredTerminators not implemented");
}

}
