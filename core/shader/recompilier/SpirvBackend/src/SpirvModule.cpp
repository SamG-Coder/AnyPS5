#include <SpirvBackend/SpirvModule.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

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

}
