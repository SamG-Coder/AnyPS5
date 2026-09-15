#include "Optimization/SrtWalker.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

void SrtWalker::BuildPlan(IrProgram& program) const {
    throw std::runtime_error("SrtWalker::BuildPlan not implemented");
}

bool SrtWalker::ValidateRuntimeValue(const IrResourcePlan& program, const IrValue* value, RuntimeValueType type) const {
    throw std::runtime_error("SrtWalker::ValidateRuntimeValue not implemented");
}

void SrtWalker::EvaluateUniformValues(const IrResourcePlan& program, std::span<IrValue* const> values, const SrtRuntime& runtime, std::span<std::uint32_t> results) const {
    throw std::runtime_error("SrtWalker::EvaluateUniformValues not implemented");
}

void SrtWalker::EvaluateDescriptorSource(const IrResourcePlan& program, std::uint32_t source, const SrtRuntime& runtime, DescriptorValue& result) const {
    throw std::runtime_error("SrtWalker::EvaluateDescriptorSource not implemented");
}

void SrtWalker::EvaluateDescriptorSources(const IrResourcePlan& program, std::span<const std::uint32_t> sources, const SrtRuntime& runtime, std::vector<DescriptorValue>& results) const {
    throw std::runtime_error("SrtWalker::EvaluateDescriptorSources not implemented");
}

void SrtWalker::EvaluateRuntimeSources(const IrResourcePlan& program, std::span<const std::uint32_t> sources, const SrtRuntime& runtime, std::vector<DescriptorValue>& results, std::vector<std::uint32_t>& flat, std::span<const std::uint8_t> cleanFlatSlots, std::vector<std::uint8_t>& activeSources) const {
    throw std::runtime_error("SrtWalker::EvaluateRuntimeSources not implemented");
}

void SrtWalker::Walk(const IrResourcePlan& program, const SrtRuntime& runtime, std::vector<std::uint32_t>& flat) const {
    throw std::runtime_error("SrtWalker::Walk not implemented");
}

}
