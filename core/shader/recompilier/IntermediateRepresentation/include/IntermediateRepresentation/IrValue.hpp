#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRVALUE_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRVALUE_HPP

#include <IntermediateRepresentation/IrOpcode.hpp>
#include <IntermediateRepresentation/IrType.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

class IrBlock;

class IrValue {
public:
    IrValue(IrOpcode opcode, IrType type, std::uint32_t id);

    [[nodiscard]] IrOpcode Opcode() const;
    [[nodiscard]] IrType Type() const;
    [[nodiscard]] std::uint32_t Id() const;
    [[nodiscard]] const std::vector<IrValue*>& Arguments() const;
    [[nodiscard]] const std::vector<IrValue*>& Uses() const;
    [[nodiscard]] IrBlock* Parent() const;
    [[nodiscard]] bool HasImmediate() const;
    [[nodiscard]] std::uint32_t ImmediateU32() const;

    void AddArgument(IrValue* argument);
    void ReplaceArgument(std::uint32_t index, IrValue* argument);
    void ReplaceAllUsesWith(IrValue* replacement);
    void SetImmediateU32(std::uint32_t value);
    void SetParent(IrBlock* parent);

private:
    IrOpcode opcode;
    IrType type;
    std::uint32_t id;
    std::uint32_t immediateU32;
    bool hasImmediate;
    std::vector<IrValue*> arguments;
    std::vector<IrValue*> uses;
    IrBlock* parent;
};

}

#endif
