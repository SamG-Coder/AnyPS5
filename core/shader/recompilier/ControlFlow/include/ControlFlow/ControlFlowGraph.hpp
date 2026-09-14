#ifndef SHADER_RECOMPILIER_CONTROLFLOW_CONTROLFLOWGRAPH_HPP
#define SHADER_RECOMPILIER_CONTROLFLOW_CONTROLFLOWGRAPH_HPP

#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct BasicBlock {
    std::uint32_t id;
    std::uint32_t startProgramCounter;
    std::uint32_t endProgramCounter;
    std::vector<std::uint32_t> predecessors;
    std::vector<std::uint32_t> successors;
};

struct NaturalLoop {
    std::uint32_t headerBlock;
    std::vector<std::uint32_t> blocks;
};

struct BackEdge {
    std::uint32_t sourceBlock;
    std::uint32_t targetBlock;
};

struct ControlFlowGraph {
    std::vector<BasicBlock> blocks;
    std::vector<NaturalLoop> naturalLoops;
    std::vector<BackEdge> backEdges;
    std::uint32_t entryBlock;

    [[nodiscard]] const BasicBlock& FindBlock(std::uint32_t blockId) const;
};

}

#endif
