#include "IntermediateRepresentation/IrBuilder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrValue& IrBuilder::Select(IrValue& condition, IrValue& trueValue, IrValue& falseValue) {
    throw std::runtime_error("IrBuilder::Select not implemented");
}

IrValue& IrBuilder::IEqual(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::IEqual not implemented");
}

IrValue& IrBuilder::INotEqual(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::INotEqual not implemented");
}

IrValue& IrBuilder::ULessThan(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::ULessThan not implemented");
}

IrValue& IrBuilder::UGreaterThan(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::UGreaterThan not implemented");
}

IrValue& IrBuilder::LogicalAnd(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::LogicalAnd not implemented");
}

IrValue& IrBuilder::LogicalOr(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::LogicalOr not implemented");
}

IrValue& IrBuilder::LogicalNot(IrValue& value) {
    throw std::runtime_error("IrBuilder::LogicalNot not implemented");
}

}
