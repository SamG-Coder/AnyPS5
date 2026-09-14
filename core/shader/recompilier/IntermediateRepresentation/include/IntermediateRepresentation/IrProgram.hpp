#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRPROGRAM_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRPROGRAM_HPP

#include <IntermediateRepresentation/IrBlock.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace ShaderRecompiler {

struct ShaderInfo {
    std::uint32_t scratchDwords;
    std::uint32_t sharedMemoryBytes;
    std::int32_t vertexOffsetSgpr;
    std::int32_t instanceOffsetSgpr;
};

class IrProgram {
public:
    [[nodiscard]] std::vector<std::unique_ptr<IrBlock>>& Blocks();
    [[nodiscard]] const std::vector<std::unique_ptr<IrBlock>>& Blocks() const;
    [[nodiscard]] IrBlock& EntryBlock() const;
    [[nodiscard]] ShaderInfo& Info();
    [[nodiscard]] const ShaderInfo& Info() const;
    [[nodiscard]] std::uint32_t WaveSize() const;

    void SetWaveSize(std::uint32_t waveSize);
    [[nodiscard]] IrBlock& CreateBlock();
    [[nodiscard]] IrValue& CreateValue(IrOpcode opcode, IrType type);

private:
    std::vector<std::unique_ptr<IrBlock>> blocks;
    std::vector<std::unique_ptr<IrValue>> values;
    ShaderInfo info;
    std::uint32_t waveSize;
    std::uint32_t nextValueId;
    std::uint32_t nextBlockId;
};

}

#endif
