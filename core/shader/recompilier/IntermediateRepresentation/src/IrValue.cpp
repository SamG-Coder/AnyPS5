#include "IntermediateRepresentation/IrValue.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrValue::IrValue(IrOpcode opcode, IrType type, std::uint32_t id) : opcode(opcode), type(type), id(id), hasImmediate(false), parent(nullptr) {
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
void IrValue::ReplaceArgument(std::size_t index, IrValue* argument) {
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

bool IrValue::IsEmpty() const {
    throw std::runtime_error("IrValue::IsEmpty not implemented");
}

bool IrValue::IsIdentity() const {
    throw std::runtime_error("IrValue::IsIdentity not implemented");
}

bool IrValue::IsPhi() const {
    throw std::runtime_error("IrValue::IsPhi not implemented");
}

IrValue* IrValue::Resolve() const {
    throw std::runtime_error("IrValue::Resolve not implemented");
}

bool IrValue::MayHaveSideEffects() const {
    throw std::runtime_error("IrValue::MayHaveSideEffects not implemented");
}

bool IrValue::HasUses() const {
    throw std::runtime_error("IrValue::HasUses not implemented");
}

std::size_t IrValue::UseCount() const {
    throw std::runtime_error("IrValue::UseCount not implemented");
}

std::size_t IrValue::ArgumentCount() const {
    throw std::runtime_error("IrValue::ArgumentCount not implemented");
}

std::size_t IrValue::PhiBlockCount() const {
    throw std::runtime_error("IrValue::PhiBlockCount not implemented");
}

IrValue* IrValue::Argument(std::size_t index) const {
    throw std::runtime_error("IrValue::Argument not implemented");
}

IrBlock* IrValue::PhiBlock(std::size_t index) const {
    throw std::runtime_error("IrValue::PhiBlock not implemented");
}

const std::vector<IrUse>& IrValue::OperandUses() const {
    throw std::runtime_error("IrValue::OperandUses not implemented");
}

std::uint64_t IrValue::ImmediateU64() const {
    throw std::runtime_error("IrValue::ImmediateU64 not implemented");
}

std::uint16_t IrValue::ImmediateF16Bits() const {
    throw std::runtime_error("IrValue::ImmediateF16Bits not implemented");
}

float IrValue::ImmediateF32() const {
    throw std::runtime_error("IrValue::ImmediateF32 not implemented");
}

bool IrValue::ImmediateBool() const {
    throw std::runtime_error("IrValue::ImmediateBool not implemented");
}

std::uint8_t IrValue::ImmediateU8() const {
    throw std::runtime_error("IrValue::ImmediateU8 not implemented");
}

std::uint16_t IrValue::ImmediateU16() const {
    throw std::runtime_error("IrValue::ImmediateU16 not implemented");
}

GuestRegister IrValue::Register() const {
    throw std::runtime_error("IrValue::Register not implemented");
}

void IrValue::SetImmediateU64(std::uint64_t value) {
    throw std::runtime_error("IrValue::SetImmediateU64 not implemented");
}

void IrValue::SetImmediateF16Bits(std::uint16_t bits) {
    throw std::runtime_error("IrValue::SetImmediateF16Bits not implemented");
}

void IrValue::SetImmediateF32(float value) {
    throw std::runtime_error("IrValue::SetImmediateF32 not implemented");
}

void IrValue::SetImmediateBool(bool value) {
    throw std::runtime_error("IrValue::SetImmediateBool not implemented");
}

void IrValue::SetImmediateU8(std::uint8_t value) {
    throw std::runtime_error("IrValue::SetImmediateU8 not implemented");
}

void IrValue::SetImmediateU16(std::uint16_t value) {
    throw std::runtime_error("IrValue::SetImmediateU16 not implemented");
}

void IrValue::SetRegister(const GuestRegister& reg) {
    throw std::runtime_error("IrValue::SetRegister not implemented");
}

void IrValue::AddPhiOperand(IrBlock* predecessor, IrValue* value) {
    throw std::runtime_error("IrValue::AddPhiOperand not implemented");
}

void IrValue::ReplaceOpcode(IrOpcode opcode) {
    throw std::runtime_error("IrValue::ReplaceOpcode not implemented");
}

void IrValue::Invalidate() {
    throw std::runtime_error("IrValue::Invalidate not implemented");
}

void IrValue::ReplaceUsesWith(IrValue* replacement, bool preserve) {
    throw std::runtime_error("IrValue::ReplaceUsesWith not implemented");
}

bool IrValue::operator==(const IrValue& other) const {
    throw std::runtime_error("operator== not implemented");
}

bool IrUse::operator==(const IrUse& other) const {
    throw std::runtime_error("IrUse::operator== not implemented");
}

}
