#include <Optimization/SsaBuilder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void SsaBuilder::Rewrite(IrProgram& program) const {
    throw std::runtime_error("SsaBuilder::Rewrite not implemented");
}
void SsaBuilder::insertPhiNodes(IrProgram& program) const {
    throw std::runtime_error("SsaBuilder::insertPhiNodes not implemented");
}
void SsaBuilder::renameVariables(IrProgram& program) const {
    throw std::runtime_error("SsaBuilder::renameVariables not implemented");
}

}
