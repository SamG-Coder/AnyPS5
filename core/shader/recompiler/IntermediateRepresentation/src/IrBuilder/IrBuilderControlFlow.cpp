#include "IntermediateRepresentation/IrBuilder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void IrBuilder::Branch(IrBlock& target) {
    throw std::runtime_error("IrBuilder::Branch not implemented");
}

void IrBuilder::BranchConditional(IrValue& condition, IrBlock& trueTarget, IrBlock& falseTarget) {
    throw std::runtime_error("IrBuilder::BranchConditional not implemented");
}

}
