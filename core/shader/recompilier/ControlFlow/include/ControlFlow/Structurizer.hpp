#ifndef SHADER_RECOMPILIER_CONTROLFLOW_STRUCTURIZER_HPP
#define SHADER_RECOMPILIER_CONTROLFLOW_STRUCTURIZER_HPP

#include <ControlFlow/ControlFlowGraph.hpp>

namespace ShaderRecompiler {

class Structurizer {
public:
    void Structurize(ControlFlowGraph& graph) const;

private:
    void computeDominatorTree(const ControlFlowGraph& graph) const;
    void detectNaturalLoops(ControlFlowGraph& graph) const;
    void verifyReducibility(const ControlFlowGraph& graph) const;
};

}

#endif
