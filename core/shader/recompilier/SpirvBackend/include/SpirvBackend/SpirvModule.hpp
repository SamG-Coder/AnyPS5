#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVMODULE_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVMODULE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ShaderRecompiler {

class SpirvModule {
public:
    [[nodiscard]] std::uint32_t AllocateId();
    void EmitCapability(std::uint32_t capability);
    void EmitExtension(const std::string& extensionName);
    void EmitEntryPoint(std::uint32_t executionModel, std::uint32_t entryPointId, const std::string& entryPointName, const std::vector<std::uint32_t>& interfaceIds);
    void EmitTypeDeclaration(std::vector<std::uint32_t> words);
    void EmitGlobalVariable(std::vector<std::uint32_t> words);
    void EmitFunctionInstruction(std::vector<std::uint32_t> words);
    [[nodiscard]] std::vector<std::uint32_t> Finalize() const;

private:
    std::uint32_t nextId;
    std::vector<std::uint32_t> capabilities;
    std::vector<std::uint32_t> extensions;
    std::vector<std::uint32_t> entryPoints;
    std::vector<std::uint32_t> typeDeclarations;
    std::vector<std::uint32_t> globalVariables;
    std::vector<std::uint32_t> functionInstructions;
};

}

#endif
