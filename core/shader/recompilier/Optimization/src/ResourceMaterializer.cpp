#include "Optimization/ResourceMaterializer.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void ResourceMaterializer::Apply(IrProgram& program, const ResourceSpecialization& specialization) const {
    throw std::runtime_error("ResourceMaterializer::Apply not implemented");
}

IrResourcePlan ResourceMaterializer::ExtractPlan(const IrProgram& program) const {
    throw std::runtime_error("ResourceMaterializer::ExtractPlan not implemented");
}

void ResourceMaterializer::Materialize(const IrResourcePlan& program, const SrtRuntime& runtime, ResourceSnapshot& snapshot, ResourceSpecialization& specialization) const {
    throw std::runtime_error("ResourceMaterializer::Materialize not implemented");
}

bool ResourceSpecialization::Buffer::operator==(const Buffer& other) const {
    throw std::runtime_error("ResourceSpecialization::Buffer::operator== not implemented");
}

bool ResourceSpecialization::Image::operator==(const Image& other) const {
    throw std::runtime_error("ResourceSpecialization::Image::operator== not implemented");
}

bool ResourceSpecialization::operator==(const ResourceSpecialization& other) const {
    throw std::runtime_error("ResourceSpecialization::operator== not implemented");
}

}
