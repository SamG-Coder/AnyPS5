#include <Optimization/DeadCodeEliminator.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void DeadCodeEliminator::Eliminate(IrProgram& program) const {
    throw std::runtime_error("DeadCodeEliminator::Eliminate not implemented");
}

}
