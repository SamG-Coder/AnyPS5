#ifndef SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVEMITTER_HPP
#define SHADER_RECOMPILIER_SPIRVBACKEND_SPIRVEMITTER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <Optimization/BindingAllocator.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct SpirvTargetOptions {
    std::uint32_t vulkanVersion;
    std::uint32_t spirvVersion;
    std::uint32_t subgroupSize;
};

class SpirvEmitter {
public:
    [[nodiscard]] std::vector<std::uint32_t> Emit(const IrProgram& program, const BindingAllocationResult& bindings, const SpirvTargetOptions& target) const;
};

}

#endif
