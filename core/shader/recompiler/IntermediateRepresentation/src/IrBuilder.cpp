#include "IntermediateRepresentation/IrBuilder.hpp"
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

IrValue& IrBuilder::ConstantU64(std::uint64_t value) {
    throw std::runtime_error("IrBuilder::ConstantU64 not implemented");
}

IrValue& IrBuilder::ConstantF16(std::uint16_t bits) {
    throw std::runtime_error("IrBuilder::ConstantF16 not implemented");
}

IrValue& IrBuilder::ConstantF32(float value) {
    throw std::runtime_error("IrBuilder::ConstantF32 not implemented");
}

IrValue& IrBuilder::ConstantBool(bool value) {
    throw std::runtime_error("IrBuilder::ConstantBool not implemented");
}

IrValue& IrBuilder::ConstantU8(std::uint8_t value) {
    throw std::runtime_error("IrBuilder::ConstantU8 not implemented");
}

IrValue& IrBuilder::ConstantU16(std::uint16_t value) {
    throw std::runtime_error("IrBuilder::ConstantU16 not implemented");
}

IrValue& IrBuilder::Emit(IrOpcode opcode, IrType type, std::initializer_list<IrValue*> arguments, std::uint64_t flags) {
    throw std::runtime_error("IrBuilder::Emit not implemented");
}

IrValue& IrBuilder::GetUserData(ScalarReg reg) {
    throw std::runtime_error("IrBuilder::GetUserData not implemented");
}

IrValue& IrBuilder::GetScalarReg(ScalarReg reg) {
    throw std::runtime_error("IrBuilder::GetScalarReg not implemented");
}

void IrBuilder::SetScalarReg(ScalarReg reg, IrValue& value) {
    throw std::runtime_error("IrBuilder::SetScalarReg not implemented");
}

IrValue& IrBuilder::GetThreadBitScalarReg(ScalarReg reg) {
    throw std::runtime_error("IrBuilder::GetThreadBitScalarReg not implemented");
}

void IrBuilder::SetThreadBitScalarReg(ScalarReg reg, IrValue& value) {
    throw std::runtime_error("IrBuilder::SetThreadBitScalarReg not implemented");
}

IrValue& IrBuilder::GetScalarMaskTag(ScalarReg reg) {
    throw std::runtime_error("IrBuilder::GetScalarMaskTag not implemented");
}

void IrBuilder::SetScalarMaskTag(ScalarReg reg, IrValue& value) {
    throw std::runtime_error("IrBuilder::SetScalarMaskTag not implemented");
}

IrValue& IrBuilder::GetVectorReg(VectorReg reg) {
    throw std::runtime_error("IrBuilder::GetVectorReg not implemented");
}

void IrBuilder::SetVectorReg(VectorReg reg, IrValue& value) {
    throw std::runtime_error("IrBuilder::SetVectorReg not implemented");
}

IrValue& IrBuilder::GetGotoVariable(std::uint32_t id) {
    throw std::runtime_error("IrBuilder::GetGotoVariable not implemented");
}

void IrBuilder::SetGotoVariable(std::uint32_t id, IrValue& value) {
    throw std::runtime_error("IrBuilder::SetGotoVariable not implemented");
}

IrValue& IrBuilder::GetScc() {
    throw std::runtime_error("IrBuilder::GetScc not implemented");
}

void IrBuilder::SetScc(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetScc not implemented");
}

IrValue& IrBuilder::GetExec() {
    throw std::runtime_error("IrBuilder::GetExec not implemented");
}

void IrBuilder::SetExec(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetExec not implemented");
}

IrValue& IrBuilder::GetExecLo() {
    throw std::runtime_error("IrBuilder::GetExecLo not implemented");
}

void IrBuilder::SetExecLo(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetExecLo not implemented");
}

IrValue& IrBuilder::GetExecHi() {
    throw std::runtime_error("IrBuilder::GetExecHi not implemented");
}

void IrBuilder::SetExecHi(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetExecHi not implemented");
}

IrValue& IrBuilder::GetVcc() {
    throw std::runtime_error("IrBuilder::GetVcc not implemented");
}

void IrBuilder::SetVcc(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetVcc not implemented");
}

IrValue& IrBuilder::GetVccLo() {
    throw std::runtime_error("IrBuilder::GetVccLo not implemented");
}

void IrBuilder::SetVccLo(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetVccLo not implemented");
}

IrValue& IrBuilder::GetVccHi() {
    throw std::runtime_error("IrBuilder::GetVccHi not implemented");
}

void IrBuilder::SetVccHi(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetVccHi not implemented");
}

IrValue& IrBuilder::GetM0() {
    throw std::runtime_error("IrBuilder::GetM0 not implemented");
}

void IrBuilder::SetM0(IrValue& value) {
    throw std::runtime_error("IrBuilder::SetM0 not implemented");
}

IrValue& IrBuilder::BitCastF32(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastF32 not implemented");
}

IrValue& IrBuilder::BitCastU32(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastU32 not implemented");
}

IrValue& IrBuilder::BitCastF16(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastF16 not implemented");
}

IrValue& IrBuilder::BitCastU32FromF16(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastU32FromF16 not implemented");
}

IrValue& IrBuilder::ConstructU64(IrValue& low, IrValue& high) {
    throw std::runtime_error("IrBuilder::ConstructU64 not implemented");
}

IrValue& IrBuilder::CompositeExtract(IrValue& composite, std::uint32_t index) {
    throw std::runtime_error("IrBuilder::CompositeExtract not implemented");
}

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
