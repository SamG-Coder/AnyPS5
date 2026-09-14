#include "Optimization/ShaderInfoCollector.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void ShaderInfoCollector::Collect(IrProgram& program) const {
    throw std::runtime_error("ShaderInfoCollector::Collect not implemented");
}

void ShaderInfoCollector::Collect(IrProgram& program, const ShaderStageInputInfo& inputInfo) const {
    throw std::runtime_error("ShaderInfoCollector::Collect not implemented");
}

}
