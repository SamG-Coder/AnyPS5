#include <Optimization/ResourceMaterializer.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

void ResourceMaterializer::Apply(IrProgram& program, const ResourceSpecialization& specialization) const {
    throw std::runtime_error("ResourceMaterializer::Apply not implemented");
}

}
