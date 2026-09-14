#ifndef SHADER_RECOMPILIER_OPTIMIZATION_BINDINGALLOCATOR_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_BINDINGALLOCATOR_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <shader/recompilier/Recompiler.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct BindingAllocationResult {
    std::vector<DescriptorBinding> bindings;
    std::uint32_t pushConstantOffsetBytes;
    std::uint32_t pushConstantSizeBytes;
};

class BindingAllocator {
public:
    [[nodiscard]] BindingAllocationResult Allocate(IrProgram& program, std::uint32_t pushDataStartDword) const;
};

}

#endif
