#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRBLOCK_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRBLOCK_HPP

#include <IntermediateRepresentation/IrValue.hpp>
#include <cstdint>
#include <list>
#include <vector>

namespace ShaderRecompiler {

class IrBlock {
public:
    explicit IrBlock(std::uint32_t id);

    [[nodiscard]] std::uint32_t Id() const;
    [[nodiscard]] std::list<IrValue*>& Instructions();
    [[nodiscard]] const std::list<IrValue*>& Instructions() const;
    [[nodiscard]] std::vector<IrBlock*>& Predecessors();
    [[nodiscard]] std::vector<IrBlock*>& Successors();

    void AppendInstruction(IrValue* value);
    void InsertInstructionBefore(IrValue* position, IrValue* value);
    void RemoveInstruction(IrValue* value);
    void AddPredecessor(IrBlock* block);
    void AddSuccessor(IrBlock* block);

private:
    std::uint32_t id;
    std::list<IrValue*> instructions;
    std::vector<IrBlock*> predecessors;
    std::vector<IrBlock*> successors;
};

}

#endif
