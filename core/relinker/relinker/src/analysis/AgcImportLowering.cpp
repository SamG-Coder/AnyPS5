#include <relinker/analysis/AgcImportLowering.hpp>
#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <unordered_map>
namespace Relinker {
void AgcImportLowering::Apply(std::vector<NidReference>& references) {
    static const std::unordered_map<std::string, std::string> native = {
        {"sceAgcDcbDrawIndex", "aps5NativeAgcDrawIndex"},
        {"sceAgcDcbDrawIndexAuto", "aps5NativeAgcDrawIndexAuto"},
        {"sceAgcDcbDrawIndexOffset", "aps5NativeAgcDrawIndexOffset"},
        {"sceAgcDcbSetIndexBuffer", "aps5NativeAgcSetIndexBuffer"},
        {"sceAgcDcbSetIndexCount", "aps5NativeAgcSetIndexCount"},
        {"sceAgcDcbSetIndexSize", "aps5NativeAgcSetIndexSize"},
        {"sceAgcDcbSetNumInstances", "aps5NativeAgcSetNumInstances"},
        {"sceAgcDriverSubmitDcb", "aps5NativeAgcSubmit"}
    };
    for (auto& reference : references) {
        if (!AgcLoweringAnalyzer::IsAgcLibrary(reference.Library)) continue;
        const auto found = native.find(reference.Nid);
        if (found != native.end()) reference.Nid = found->second;
    }
}
}
