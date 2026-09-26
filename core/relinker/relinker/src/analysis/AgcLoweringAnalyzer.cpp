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
    std::string failures;
    for (const auto& entry : entries) {
        if (!IsAgcLibrary(entry.Library)) continue;
        const auto reject = [&](const char* reason) {
            if (!failures.empty()) failures += '\n';
            failures += std::string(reason) + entry.Nid + " [" + entry.Library + "]";
        };
        const auto kind=classify(entry);
        if (kind==AgcLoweringKind::Other) {
            if (!entry.CallSitesResolved || entry.CallSites.empty())
                reject("AGC import has indirect or unresolved call sites: ");
            reject("AGC import has no native lowering: ");
            continue;
        }
        for(const auto site:entry.CallSites) result.push_back({site,entry.Nid,entry.Library,kind});
    }
    if (!failures.empty()) throw RelinkerException(failures);
    std::sort(result.begin(),result.end(),[](const auto&a,const auto&b){return a.CallSite<b.CallSite;});
    return result;
}
}
