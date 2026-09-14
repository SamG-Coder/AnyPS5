#include <Translation/EmbeddedVertexFetch.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

EmbeddedFetchPlan EmbeddedVertexFetchAnalyzer::Analyze(const RdnaProgram& program, std::uint32_t attributeTableRegister, std::uint32_t bufferTableRegister, std::uint32_t userDataBaseRegister, std::uint32_t userDataCount, std::uint32_t waveSize) const {
    throw std::runtime_error("EmbeddedVertexFetchAnalyzer::Analyze not implemented");
}

}
