#include <Optimization/ResourceTracker.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void ResourceTracker::Track(IrProgram& program) const {
    throw std::runtime_error("ResourceTracker::Track not implemented");
}

}
