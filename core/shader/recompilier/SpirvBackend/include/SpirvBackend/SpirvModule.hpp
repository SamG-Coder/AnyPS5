#ifndef CORE_SHADER_RECOMPILIER_SPIRVBACKEND_INCLUDE_SPIRVBACKEND_SPIRVMODULE_HPP
#define CORE_SHADER_RECOMPILIER_SPIRVBACKEND_INCLUDE_SPIRVBACKEND_SPIRVMODULE_HPP

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <map>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace ShaderRecompiler {

struct SpirvTypeAnnotation {
    std::uint32_t opcode = 0;
    std::vector<std::uint32_t> operands;
};

struct SpirvDeferredPhi {
    std::size_t wordOffset = 0;
};

class SpirvModule {
public:
    explicit SpirvModule(std::uint32_t version = 0x00010300u);
    [[nodiscard]] std::uint32_t AllocateId();
    void EmitCapability(std::uint32_t capability);
    void EmitExtension(const std::string& extensionName);
    void EmitEntryPoint(std::uint32_t executionModel, std::uint32_t entryPointId, const std::string& entryPointName, const std::vector<std::uint32_t>& interfaceIds);
    void EmitTypeDeclaration(std::vector<std::uint32_t> words);
    void EmitGlobalVariable(std::vector<std::uint32_t> words);
    void EmitFunctionInstruction(std::vector<std::uint32_t> words);
    [[nodiscard]] std::vector<std::uint32_t> Finalize() const;

    void RequireVersion(std::uint32_t version);
    [[nodiscard]] std::uint32_t Import(const std::string& name);
    [[nodiscard]] std::uint32_t DefineGlobalVariable(std::uint32_t pointerType, std::uint32_t storageClass);
    void DefineGlobalVariable(std::uint32_t id, std::uint32_t pointerType, std::uint32_t storageClass);
    void AddMemoryModel(std::uint32_t addressingModel, std::uint32_t memoryModel);
    void AddName(std::uint32_t target, const std::string& name);
    void AddFunction(std::span<const std::uint32_t> words);
    [[nodiscard]] SpirvDeferredPhi AddDeferredPhi(std::uint32_t type, std::uint32_t result, std::size_t incomingCount);
    void PatchDeferredPhi(SpirvDeferredPhi phi, std::size_t incoming, std::uint32_t value, std::uint32_t parent);

    template<typename... TOperands>
    std::uint32_t Type(std::uint32_t opcode, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::Type not implemented");
    }

    template<typename... TOperands>
    std::uint32_t DecoratedType(std::uint32_t opcode, std::initializer_list<SpirvTypeAnnotation> annotations, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::DecoratedType not implemented");
    }

    template<typename... TOperands>
    std::uint32_t Constant(std::uint32_t opcode, std::uint32_t type, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::Constant not implemented");
    }

    template<typename... TOperands>
    void AddExecutionMode(std::uint32_t entryPoint, std::uint32_t mode, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::AddExecutionMode not implemented");
    }

    template<typename... TOperands>
    void AddAnnotation(std::uint32_t opcode, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::AddAnnotation not implemented");
    }

    template<typename... TOperands>
    void AddFunction(std::uint32_t opcode, const TOperands&... operands) {
        throw std::runtime_error("SpirvModule::AddFunction not implemented");
    }

private:
    std::uint32_t nextId = 1;
    std::uint32_t version = 0x00010300u;
    std::vector<std::uint32_t> extInstImports;
    std::vector<std::uint32_t> memoryModel;
    std::vector<std::uint32_t> executionModes;
    std::vector<std::uint32_t> debug;
    std::vector<std::uint32_t> annotations;
    std::vector<std::uint32_t> declarations;
    std::set<std::uint32_t> requiredCapabilities;
    std::set<std::string> requiredExtensions;
    std::map<std::string, std::uint32_t> importIds;
    std::map<std::vector<std::uint32_t>, std::uint32_t> declarationIds;
    std::size_t unpatchedPhiIncomings = 0;
    std::vector<std::uint32_t> capabilities;
    std::vector<std::uint32_t> extensions;
    std::vector<std::uint32_t> entryPoints;
    std::vector<std::uint32_t> typeDeclarations;
    std::vector<std::uint32_t> globalVariables;
    std::vector<std::uint32_t> functionInstructions;
};

}

#endif
