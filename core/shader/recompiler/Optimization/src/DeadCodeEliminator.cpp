#include "Optimization/DeadCodeEliminator.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void DeadCodeEliminator::Eliminate(IrProgram& program) const {
    throw std::runtime_error("DeadCodeEliminator::Eliminate not implemented");
}

void DeadCodeEliminator::Eliminate(std::span<IrBlock* const> blocks) const {
    throw std::runtime_error("DeadCodeEliminator::Eliminate not implemented");
}

void DeadCodeEliminator::RemoveIdentities(IrProgram& program) const {
    throw std::runtime_error("DeadCodeEliminator::RemoveIdentities not implemented");
}

void DeadCodeEliminator::RemoveIdentities(std::span<IrBlock* const> blocks) const {
    throw std::runtime_error("DeadCodeEliminator::RemoveIdentities not implemented");
}

}
