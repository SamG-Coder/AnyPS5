#ifndef SHADER_RECOMPILIER_OPTIMIZATION_SHAREDMEMORYBARRIERINSERTER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_SHAREDMEMORYBARRIERINSERTER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <cstdint>

namespace ShaderRecompiler {

struct SharedMemoryBarrierStats {
    std::uint32_t insertedBarriers;
};

class SharedMemoryBarrierInserter {
public:
    [[nodiscard]] SharedMemoryBarrierStats Insert(IrProgram& program, std::uint32_t waveSize) const;
};

}

#endif
