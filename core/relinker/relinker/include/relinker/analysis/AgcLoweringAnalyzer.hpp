#ifndef RELINKER_ANALYSIS_AGCLOWERINGANALYZER_HPP
#define RELINKER_ANALYSIS_AGCLOWERINGANALYZER_HPP
#include <relinker/domain/Types.hpp>
#include <string>
#include <vector>
namespace Relinker {
enum class AgcLoweringKind { CommandProducer, CommandPatcher, Submission, ShaderCreation, Other };
struct AgcLoweringSite { FileByteOffset CallSite; std::string Nid; std::string Library; AgcLoweringKind Kind; };
class AgcLoweringAnalyzer {
public:
    std::vector<AgcLoweringSite> Analyze(const std::vector<CallRegistryEntry>& entries) const;
    static bool IsAgcLibrary(const std::string& library);
};
}
#endif
