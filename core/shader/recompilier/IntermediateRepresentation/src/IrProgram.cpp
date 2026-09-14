#include <IntermediateRepresentation/IrProgram.hpp>
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

}
