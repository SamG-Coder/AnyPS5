#include "IntermediateRepresentation/IrBuilder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrValue& IrBuilder::IAdd(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::IAdd not implemented");
}

IrValue& IrBuilder::ISub(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::ISub not implemented");
}

IrValue& IrBuilder::IMul(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::IMul not implemented");
}

IrValue& IrBuilder::ShiftLeftLogical(IrValue& value, IrValue& shift) {
    throw std::runtime_error("IrBuilder::ShiftLeftLogical not implemented");
}

IrValue& IrBuilder::ShiftRightLogical(IrValue& value, IrValue& shift) {
    throw std::runtime_error("IrBuilder::ShiftRightLogical not implemented");
}

IrValue& IrBuilder::ShiftRightArithmetic(IrValue& value, IrValue& shift) {
    throw std::runtime_error("IrBuilder::ShiftRightArithmetic not implemented");
}

IrValue& IrBuilder::BitwiseAnd(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::BitwiseAnd not implemented");
}

IrValue& IrBuilder::BitwiseOr(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::BitwiseOr not implemented");
}

IrValue& IrBuilder::BitwiseXor(IrValue& lhs, IrValue& rhs) {
    throw std::runtime_error("IrBuilder::BitwiseXor not implemented");
}

IrValue& IrBuilder::BitwiseNot(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitwiseNot not implemented");
}

}
