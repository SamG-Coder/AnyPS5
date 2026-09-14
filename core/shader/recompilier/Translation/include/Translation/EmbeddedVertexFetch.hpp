#ifndef SHADER_RECOMPILIER_TRANSLATION_EMBEDDEDVERTEXFETCH_HPP
#define SHADER_RECOMPILIER_TRANSLATION_EMBEDDEDVERTEXFETCH_HPP

#include <RdnaDecoder/RdnaProgram.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct EmbeddedFetchLoad {
    std::uint32_t programCounter;
    std::int32_t attributeId;
    std::uint32_t componentCount;
};

struct EmbeddedFetchPlan {
    std::vector<EmbeddedFetchLoad> loads;
    std::int32_t vertexOffsetSgpr;
    std::int32_t instanceOffsetSgpr;
};

class EmbeddedVertexFetchAnalyzer {
public:
    [[nodiscard]] EmbeddedFetchPlan Analyze(const RdnaProgram& program, std::uint32_t attributeTableRegister, std::uint32_t bufferTableRegister, std::uint32_t userDataBaseRegister, std::uint32_t userDataCount, std::uint32_t waveSize) const;
};

}

#endif
