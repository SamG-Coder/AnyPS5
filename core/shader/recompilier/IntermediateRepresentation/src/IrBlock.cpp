#include <IntermediateRepresentation/IrBlock.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

IrBlock::IrBlock(std::uint32_t id) : id(id) {
    throw std::runtime_error("IrBlock::IrBlock not implemented");
}
std::uint32_t IrBlock::Id() const {
    throw std::runtime_error("IrBlock::Id not implemented");
}
std::list<IrValue*>& IrBlock::Instructions() {
    throw std::runtime_error("IrBlock::Instructions not implemented");
}
const std::list<IrValue*>& IrBlock::Instructions() const {
    throw std::runtime_error("IrBlock::Instructions not implemented");
}
std::vector<IrBlock*>& IrBlock::Predecessors() {
    throw std::runtime_error("IrBlock::Predecessors not implemented");
}
std::vector<IrBlock*>& IrBlock::Successors() {
    throw std::runtime_error("IrBlock::Successors not implemented");
}
void IrBlock::AppendInstruction(IrValue* value) {
    throw std::runtime_error("IrBlock::AppendInstruction not implemented");
}
void IrBlock::InsertInstructionBefore(IrValue* position, IrValue* value) {
    throw std::runtime_error("IrBlock::InsertInstructionBefore not implemented");
}
void IrBlock::RemoveInstruction(IrValue* value) {
    throw std::runtime_error("IrBlock::RemoveInstruction not implemented");
}
void IrBlock::AddPredecessor(IrBlock* block) {
    throw std::runtime_error("IrBlock::AddPredecessor not implemented");
}
void IrBlock::AddSuccessor(IrBlock* block) {
    throw std::runtime_error("IrBlock::AddSuccessor not implemented");
}

}
