#include "SpirvBackend/SpirvModule.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

SpirvModule::SpirvModule(std::uint32_t version) : version(version) {
    throw std::runtime_error("SpirvModule not implemented");
}

std::uint32_t SpirvModule::AllocateId() {
    throw std::runtime_error("SpirvModule::AllocateId not implemented");
}
void SpirvModule::EmitCapability(std::uint32_t capability) {
    throw std::runtime_error("SpirvModule::EmitCapability not implemented");
}
void SpirvModule::EmitExtension(const std::string& extensionName) {
    throw std::runtime_error("SpirvModule::EmitExtension not implemented");
}
void SpirvModule::EmitEntryPoint(std::uint32_t executionModel, std::uint32_t entryPointId, const std::string& entryPointName, const std::vector<std::uint32_t>& interfaceIds) {
    throw std::runtime_error("SpirvModule::EmitEntryPoint not implemented");
}
void SpirvModule::EmitTypeDeclaration(std::vector<std::uint32_t> words) {
    throw std::runtime_error("SpirvModule::EmitTypeDeclaration not implemented");
}
void SpirvModule::EmitGlobalVariable(std::vector<std::uint32_t> words) {
    throw std::runtime_error("SpirvModule::EmitGlobalVariable not implemented");
}
void SpirvModule::EmitFunctionInstruction(std::vector<std::uint32_t> words) {
    throw std::runtime_error("SpirvModule::EmitFunctionInstruction not implemented");
}
std::vector<std::uint32_t> SpirvModule::Finalize() const {
    throw std::runtime_error("SpirvModule::Finalize not implemented");
}

void SpirvModule::RequireVersion(std::uint32_t version) {
    throw std::runtime_error("SpirvModule::RequireVersion not implemented");
}

std::uint32_t SpirvModule::Import(const std::string& name) {
    throw std::runtime_error("SpirvModule::Import not implemented");
}

std::uint32_t SpirvModule::DefineGlobalVariable(std::uint32_t pointerType, std::uint32_t storageClass) {
    throw std::runtime_error("SpirvModule::DefineGlobalVariable not implemented");
}

void SpirvModule::DefineGlobalVariable(std::uint32_t id, std::uint32_t pointerType, std::uint32_t storageClass) {
    throw std::runtime_error("SpirvModule::DefineGlobalVariable not implemented");
}

void SpirvModule::AddMemoryModel(std::uint32_t addressingModel, std::uint32_t memoryModel) {
    throw std::runtime_error("SpirvModule::AddMemoryModel not implemented");
}

void SpirvModule::AddName(std::uint32_t target, const std::string& name) {
    throw std::runtime_error("SpirvModule::AddName not implemented");
}

void SpirvModule::AddFunction(std::span<const std::uint32_t> words) {
    throw std::runtime_error("SpirvModule::AddFunction not implemented");
}

SpirvDeferredPhi SpirvModule::AddDeferredPhi(std::uint32_t type, std::uint32_t result, std::size_t incomingCount) {
    throw std::runtime_error("SpirvModule::AddDeferredPhi not implemented");
}

void SpirvModule::PatchDeferredPhi(SpirvDeferredPhi phi, std::size_t incoming, std::uint32_t value, std::uint32_t parent) {
    throw std::runtime_error("SpirvModule::PatchDeferredPhi not implemented");
}

}
