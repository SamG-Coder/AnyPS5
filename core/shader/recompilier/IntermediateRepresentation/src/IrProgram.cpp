#include "IntermediateRepresentation/IrProgram.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

std::vector<std::unique_ptr<IrBlock>>& IrProgram::Blocks() {
    throw std::runtime_error("IrProgram::Blocks not implemented");
}
const std::vector<std::unique_ptr<IrBlock>>& IrProgram::Blocks() const {
    throw std::runtime_error("IrProgram::Blocks not implemented");
}
IrBlock& IrProgram::EntryBlock() const {
    throw std::runtime_error("IrProgram::EntryBlock not implemented");
}
ShaderInfo& IrProgram::Info() {
    throw std::runtime_error("IrProgram::Info not implemented");
}
const ShaderInfo& IrProgram::Info() const {
    throw std::runtime_error("IrProgram::Info not implemented");
}
std::uint32_t IrProgram::WaveSize() const {
    throw std::runtime_error("IrProgram::WaveSize not implemented");
}
void IrProgram::SetWaveSize(std::uint32_t waveSize) {
    throw std::runtime_error("IrProgram::SetWaveSize not implemented");
}
IrBlock& IrProgram::CreateBlock() {
    throw std::runtime_error("IrProgram::CreateBlock not implemented");
}
IrValue& IrProgram::CreateValue(IrOpcode opcode, IrType type) {
    throw std::runtime_error("IrProgram::CreateValue not implemented");
}

IrResourcePlan& IrProgram::Resources() {
    throw std::runtime_error("IrProgram::Resources not implemented");
}

const IrResourcePlan& IrProgram::Resources() const {
    throw std::runtime_error("IrProgram::Resources not implemented");
}

IrProgramMetadata& IrProgram::Metadata() {
    throw std::runtime_error("IrProgram::Metadata not implemented");
}

const IrProgramMetadata& IrProgram::Metadata() const {
    throw std::runtime_error("IrProgram::Metadata not implemented");
}

CompiledShaderInfo IrProgram::TakeCompiledInfo() && {
    throw std::runtime_error("IrProgram::TakeCompiledInfo not implemented");
}

IrValue& IrProgram::CreateValue(IrOpcode opcode, IrType type, std::uint64_t flags) {
    throw std::runtime_error("IrProgram::CreateValue not implemented");
}

void IrProgram::SetEntryBlock(IrBlock& block) {
    throw std::runtime_error("IrProgram::SetEntryBlock not implemented");
}

std::string ProgramToString(const IrProgram& program) {
    throw std::runtime_error("ProgramToString not implemented");
}

void ValidateProgram(const IrProgram& program, bool requireSsa) {
    throw std::runtime_error("ValidateProgram not implemented");
}

void ResolveControlFlowIdentities(IrProgram& program) {
    throw std::runtime_error("ResolveControlFlowIdentities not implemented");
}

bool EquivalentValue(const IrResourcePlan& program, const IrValue* left, const IrValue* right) {
    throw std::runtime_error("EquivalentValue not implemented");
}

IrValue* ResolveInvariantPhi(const IrResourcePlan& program, IrValue* value) {
    throw std::runtime_error("ResolveInvariantPhi not implemented");
}

bool IsAddressResourceKind(ResourceKind kind) {
    throw std::runtime_error("IsAddressResourceKind not implemented");
}

PositionExportComponent DecodePositionExportComponent(std::uint32_t control, std::uint32_t positionIndex, std::uint32_t component) {
    throw std::runtime_error("DecodePositionExportComponent not implemented");
}

std::uint32_t NativeBinding(IrShaderStage stage, DescriptorBindingKind kind) {
    throw std::runtime_error("NativeBinding not implemented");
}

ImageResourceClass ImageBindingResourceClass(DescriptorBindingKind kind) {
    throw std::runtime_error("ImageBindingResourceClass not implemented");
}

std::uint32_t ImageBindingIndex(DescriptorBindingKind kind) {
    throw std::runtime_error("ImageBindingIndex not implemented");
}

DescriptorBindingKind DescriptorBindingForImage(const ImageResource& image) {
    throw std::runtime_error("DescriptorBindingForImage not implemented");
}

std::vector<IrBlock*>& IrProgram::BlockOrder() {
    throw std::runtime_error("IrProgram::BlockOrder not implemented");
}

const std::vector<IrBlock*>& IrProgram::BlockOrder() const {
    throw std::runtime_error("IrProgram::BlockOrder not implemented");
}

}
