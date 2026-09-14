#include <IntermediateRepresentation/IrValue.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

IrValue::IrValue(IrOpcode opcode, IrType type, std::uint32_t id) : opcode(opcode), type(type), id(id), immediateU32(0), hasImmediate(false), parent(nullptr) {
    throw std::runtime_error("IrValue::IrValue not implemented");
}
IrOpcode IrValue::Opcode() const {
    throw std::runtime_error("IrValue::Opcode not implemented");
}
IrType IrValue::Type() const {
    throw std::runtime_error("IrValue::Type not implemented");
}
std::uint32_t IrValue::Id() const {
    throw std::runtime_error("IrValue::Id not implemented");
}
const std::vector<IrValue*>& IrValue::Arguments() const {
    throw std::runtime_error("IrValue::Arguments not implemented");
}
const std::vector<IrValue*>& IrValue::Uses() const {
    throw std::runtime_error("IrValue::Uses not implemented");
}
IrBlock* IrValue::Parent() const {
    throw std::runtime_error("IrValue::Parent not implemented");
}
bool IrValue::HasImmediate() const {
    throw std::runtime_error("IrValue::HasImmediate not implemented");
}
std::uint32_t IrValue::ImmediateU32() const {
    throw std::runtime_error("IrValue::ImmediateU32 not implemented");
}
void IrValue::AddArgument(IrValue* argument) {
    throw std::runtime_error("IrValue::AddArgument not implemented");
}
void IrValue::ReplaceArgument(std::uint32_t index, IrValue* argument) {
    throw std::runtime_error("IrValue::ReplaceArgument not implemented");
}
void IrValue::ReplaceAllUsesWith(IrValue* replacement) {
    throw std::runtime_error("IrValue::ReplaceAllUsesWith not implemented");
}
void IrValue::SetImmediateU32(std::uint32_t value) {
    throw std::runtime_error("IrValue::SetImmediateU32 not implemented");
}
void IrValue::SetParent(IrBlock* parent) {
    throw std::runtime_error("IrValue::SetParent not implemented");
}

}
