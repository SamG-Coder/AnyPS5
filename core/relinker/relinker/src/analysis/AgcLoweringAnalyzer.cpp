#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <relinker/analysis/AgcImportLowering.hpp>
#include <algorithm>
namespace Relinker {
bool AgcLoweringAnalyzer::IsAgcLibrary(const std::string& library) { return library.find("libSceAgc") != std::string::npos; }
static AgcLoweringKind classify(const CallRegistryEntry& entry) {
    const auto native=AgcImportLowering::NativeSymbol(entry.Nid,entry.Library);
    if (!native) return AgcLoweringKind::Other;
    if (*native=="aps5NativeAgcSubmit") return AgcLoweringKind::Submission;
    if (*native=="aps5NativeAgcCreateShader") return AgcLoweringKind::ShaderCreation;
    return AgcLoweringKind::CommandProducer;
}
std::vector<AgcLoweringSite> AgcLoweringAnalyzer::Analyze(const std::vector<CallRegistryEntry>& entries) const {
    std::vector<AgcLoweringSite> result;
    for (const auto& entry : entries) {
        if (!IsAgcLibrary(entry.Library)) continue;
        if (!entry.CallSitesResolved || entry.CallSites.empty())
            throw RelinkerException("AGC import cannot be lowered natively because its call sites are indirect or unresolved: " + entry.Nid);
        const auto kind=classify(entry);
        if (kind==AgcLoweringKind::Other)
            throw RelinkerException("AGC import has no native lowering: "+entry.Nid);
        for(const auto site:entry.CallSites) result.push_back({site,entry.Nid,entry.Library,kind});
    }
    std::sort(result.begin(),result.end(),[](const auto&a,const auto&b){return a.CallSite<b.CallSite;});
    return result;
}
}
