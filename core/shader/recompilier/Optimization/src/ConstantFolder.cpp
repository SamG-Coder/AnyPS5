#include "Optimization/ConstantFolder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void ConstantFolder::Fold(IrProgram& program) const {
    throw std::runtime_error("ConstantFolder::Fold not implemented");
}
bool ConstantFolder::tryFoldValue(IrValue& value) const {
    throw std::runtime_error("ConstantFolder::tryFoldValue not implemented");
}

void ConstantFolder::Fold(std::span<IrBlock* const> blocks) const {
    throw std::runtime_error("ConstantFolder::Fold not implemented");
}

}
