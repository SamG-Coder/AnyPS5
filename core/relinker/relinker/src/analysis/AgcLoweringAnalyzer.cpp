#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <algorithm>
namespace Relinker {
bool AgcLoweringAnalyzer::IsAgcLibrary(const std::string& library) { return library.find("libSceAgc") != std::string::npos; }
static AgcLoweringKind classify(const CallRegistryEntry& entry) {
    const auto& name = entry.Nid;
    if (name.find("Submit") != std::string::npos) return AgcLoweringKind::Submission;
    if (name.find("CreateShader") != std::string::npos || name.find("LinkShaders") != std::string::npos) return AgcLoweringKind::ShaderCreation;
    if (name.find("Patch") != std::string::npos || name.find("PayloadAddress") != std::string::npos) return AgcLoweringKind::CommandPatcher;
    if (name.find("Dcb") != std::string::npos || name.find("Cb") != std::string::npos || name.find("Acb") != std::string::npos)
        return AgcLoweringKind::CommandProducer;
    return AgcLoweringKind::Other;
}
std::vector<AgcLoweringSite> AgcLoweringAnalyzer::Analyze(const std::vector<CallRegistryEntry>& entries) const {
    std::vector<AgcLoweringSite> result;
    for (const auto& entry : entries) {
        if (!IsAgcLibrary(entry.Library)) continue;
        if (!entry.CallSitesResolved || entry.CallSites.empty())
            throw RelinkerException("AGC import cannot be lowered natively because its call sites are indirect or unresolved: " + entry.Nid);
        const auto kind = classify(entry);
        for (const auto site : entry.CallSites) result.push_back({site, entry.Nid, entry.Library, kind});
    }
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.CallSite < b.CallSite; });
    return result;
}
}
