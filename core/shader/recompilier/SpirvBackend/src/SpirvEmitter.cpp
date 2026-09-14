#include <SpirvBackend/SpirvEmitter.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

std::vector<std::uint32_t> SpirvEmitter::Emit(const IrProgram& program, const BindingAllocationResult& bindings, const SpirvTargetOptions& target) const {
    throw std::runtime_error("SpirvEmitter::Emit not implemented");
}

}
