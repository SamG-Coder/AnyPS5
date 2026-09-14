#include <Optimization/BindingAllocator.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

BindingAllocationResult BindingAllocator::Allocate(IrProgram& program, std::uint32_t pushDataStartDword) const {
    throw std::runtime_error("BindingAllocator::Allocate not implemented");
}

}
