#ifndef CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_BINDINGALLOCATOR_HPP
#define CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_BINDINGALLOCATOR_HPP

#include "IntermediateRepresentation/IrProgram.hpp"
#include "Recompiler.hpp"
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct BindingAllocationResult {
    std::vector<DescriptorBinding> bindings;
    IrBindingLayout layout;
    std::uint32_t pushConstantOffsetBytes = 0;
    std::uint32_t pushConstantSizeBytes = 0;
};

class BindingAllocator {
public:
    [[nodiscard]] BindingAllocationResult Allocate(IrProgram& program, std::uint32_t pushDataStartDword = 0) const;
    [[nodiscard]] const IrDescriptorBinding& FindBinding(const IrBindingLayout& layout, DescriptorBindingKind kind) const;

};

}

#endif
