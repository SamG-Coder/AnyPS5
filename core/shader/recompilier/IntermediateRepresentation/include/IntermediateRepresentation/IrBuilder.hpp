#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRBUILDER_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRBUILDER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <IntermediateRepresentation/GuestRegister.hpp>
#include <initializer_list>

namespace ShaderRecompiler {

class IrBuilder {
public:
    explicit IrBuilder(IrProgram& program);

    void SetInsertionPoint(IrBlock& block);
    [[nodiscard]] IrValue& Constant(std::uint32_t value);
    [[nodiscard]] IrValue& ReadRegister(const GuestRegister& reg);
    void WriteRegister(const GuestRegister& reg, IrValue& value);
    [[nodiscard]] IrValue& Emit(IrOpcode opcode, IrType type, std::initializer_list<IrValue*> arguments);
    void Branch(IrBlock& target);
    void BranchConditional(IrValue& condition, IrBlock& trueTarget, IrBlock& falseTarget);

private:
    IrProgram& program;
    IrBlock* insertionPoint;
};

}

#endif
