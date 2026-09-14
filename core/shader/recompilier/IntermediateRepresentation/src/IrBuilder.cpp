#include <IntermediateRepresentation/IrBuilder.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

IrBuilder::IrBuilder(IrProgram& program) : program(program), insertionPoint(nullptr) {
    throw std::runtime_error("IrBuilder::IrBuilder not implemented");
}
void IrBuilder::SetInsertionPoint(IrBlock& block) {
    throw std::runtime_error("IrBuilder::SetInsertionPoint not implemented");
}
IrValue& IrBuilder::Constant(std::uint32_t value) {
    throw std::runtime_error("IrBuilder::Constant not implemented");
}
IrValue& IrBuilder::ReadRegister(const GuestRegister& reg) {
    throw std::runtime_error("IrBuilder::ReadRegister not implemented");
}
void IrBuilder::WriteRegister(const GuestRegister& reg, IrValue& value) {
    throw std::runtime_error("IrBuilder::WriteRegister not implemented");
}
IrValue& IrBuilder::Emit(IrOpcode opcode, IrType type, std::initializer_list<IrValue*> arguments) {
    throw std::runtime_error("IrBuilder::Emit not implemented");
}
void IrBuilder::Branch(IrBlock& target) {
    throw std::runtime_error("IrBuilder::Branch not implemented");
}
void IrBuilder::BranchConditional(IrValue& condition, IrBlock& trueTarget, IrBlock& falseTarget) {
    throw std::runtime_error("IrBuilder::BranchConditional not implemented");
}

}
