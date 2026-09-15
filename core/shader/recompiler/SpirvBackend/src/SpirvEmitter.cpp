#include "SpirvBackend/SpirvEmitter.hpp"
#include "SpirvBackend/SpirvEmitterState.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

std::vector<std::uint32_t> SpirvEmitter::Emit(const IrProgram& program, const BindingAllocationResult& bindings, const SpirvTargetOptions& target) const {
    throw std::runtime_error("SpirvEmitter::Emit not implemented");
}

SpirvEmitterState::SpirvEmitterState(const IrProgram& program, const ShaderStageInputInfo& inputInfo) : program(program), inputInfo(inputInfo) {
    throw std::runtime_error("SpirvEmitterState not implemented");
}

SpirvValueEmitContext::SpirvValueEmitContext(SpirvEmitterState& state) : state(state) {
    throw std::runtime_error("SpirvValueEmitContext not implemented");
}

std::uint32_t SpirvValueEmitContext::Def(const IrValue* value) {
    throw std::runtime_error("SpirvValueEmitContext::Def not implemented");
}

std::uint32_t SpirvValueEmitContext::Arg(const IrValue& inst, std::size_t index) {
    throw std::runtime_error("SpirvValueEmitContext::Arg not implemented");
}

std::uint32_t SpirvValueEmitContext::HalfArg(const IrValue& inst, std::size_t index, std::uint32_t half) {
    throw std::runtime_error("SpirvValueEmitContext::HalfArg not implemented");
}

std::uint32_t SpirvValueEmitContext::Ballot(const IrValue* predicate) {
    throw std::runtime_error("SpirvValueEmitContext::Ballot not implemented");
}

std::uint32_t SpirvValueEmitContext::FirstLane(std::uint32_t ballot) {
    throw std::runtime_error("SpirvValueEmitContext::FirstLane not implemented");
}

std::uint32_t SpirvValueEmitContext::Shuffle(const IrValue& inst, std::size_t index, std::uint32_t lane) {
    throw std::runtime_error("SpirvValueEmitContext::Shuffle not implemented");
}

std::uint32_t SpirvValueEmitContext::Result(const IrValue& inst) {
    throw std::runtime_error("SpirvValueEmitContext::Result not implemented");
}

std::uint32_t SpirvValueEmitContext::Define(const IrValue& inst, std::uint32_t value) {
    throw std::runtime_error("SpirvValueEmitContext::Define not implemented");
}

std::uint32_t SpirvValueEmitContext::ResourceIndex(const IrValue* value, IrOpcode opcode) {
    throw std::runtime_error("SpirvValueEmitContext::ResourceIndex not implemented");
}

const IrValue* SpirvValueEmitContext::ImageAddress(const IrValue* value) {
    throw std::runtime_error("SpirvValueEmitContext::ImageAddress not implemented");
}

const MemoryInfo& SpirvValueEmitContext::Memory(const IrValue& inst) const {
    throw std::runtime_error("SpirvValueEmitContext::Memory not implemented");
}

const ExportInfo& SpirvValueEmitContext::Export(const IrValue& inst) const {
    throw std::runtime_error("SpirvValueEmitContext::Export not implemented");
}

std::uint32_t SpirvValueEmitContext::Label(const IrBlock* block) const {
    throw std::runtime_error("SpirvValueEmitContext::Label not implemented");
}

[[noreturn]] void SpirvValueEmitContext::Fail(const char* reason) const {
    throw std::runtime_error("SpirvValueEmitContext::Fail not implemented");
}

[[noreturn]] void SpirvValueEmitContext::Fail(const IrValue& inst, const char* reason) const {
    throw std::runtime_error("SpirvValueEmitContext::Fail not implemented");
}

std::vector<std::uint32_t> SpirvEmitter::Emit(const IrProgram& program, const ShaderStageInputInfo& inputInfo, const BindingAllocationResult& bindings, const SpirvTargetOptions& target) const {
    throw std::runtime_error("SpirvEmitter::Emit not implemented");
}

}
