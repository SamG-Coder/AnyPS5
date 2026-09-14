#include "ControlFlow/ControlFlowGraph.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

const BasicBlock& ControlFlowGraph::FindBlock(std::uint32_t blockId) const {
    throw std::runtime_error("ControlFlowGraph::FindBlock not implemented");
}

BasicBlock& ControlFlowGraph::FindBlock(std::uint32_t blockId) {
    throw std::runtime_error("ControlFlowGraph::FindBlock not implemented");
}

const BasicBlock& ControlFlowGraph::FindBlockByProgramCounter(std::uint32_t programCounter) const {
    throw std::runtime_error("ControlFlowGraph::FindBlockByProgramCounter not implemented");
}

BasicBlock& ControlFlowGraph::FindBlockByProgramCounter(std::uint32_t programCounter) {
    throw std::runtime_error("ControlFlowGraph::FindBlockByProgramCounter not implemented");
}

bool ControlFlowGraph::Dominates(std::uint32_t dominator, std::uint32_t blockId) const {
    throw std::runtime_error("ControlFlowGraph::Dominates not implemented");
}

bool ControlFlowGraph::PostDominates(std::uint32_t postDominator, std::uint32_t blockId) const {
    throw std::runtime_error("ControlFlowGraph::PostDominates not implemented");
}

std::uint32_t ControlFlowGraph::FindNearestCommonPostDominator(std::uint32_t firstBlock, std::uint32_t secondBlock) const {
    throw std::runtime_error("ControlFlowGraph::FindNearestCommonPostDominator not implemented");
}

std::string BranchConditionToString(BranchCondition condition) {
    throw std::runtime_error("BranchConditionToString not implemented");
}

std::string FailureKindToString(FailureKind kind) {
    throw std::runtime_error("FailureKindToString not implemented");
}

std::string GraphToString(const ControlFlowGraph& graph) {
    throw std::runtime_error("GraphToString not implemented");
}

}
