#include <relinker/analysis/AgcImportLowering.hpp>
#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <unordered_map>
#include <nid/NidCompute.hpp>
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
        {"sceAgcDriverSubmitDcb", "aps5NativeAgcSubmit"},
        {"sceAgcCreateShader", "aps5NativeAgcCreateShader"},
        {"sceAgcCbSetShRegistersDirect", "aps5NativeAgcSetShRegisters"},
        {"sceAgcCbSetShRegisterRangeDirect", "aps5NativeAgcSetShRegisterRange"},
        {"sceAgcCbSetUcRegistersDirect", "aps5NativeAgcSetUcRegisters"},
        {"sceAgcCbSetUcRegisterRangeDirect", "aps5NativeAgcSetUcRegisterRange"}
    };
    for (auto& reference : references) {
        if (!AgcLoweringAnalyzer::IsAgcLibrary(reference.Library)) continue;
        auto found = native.find(reference.Nid);
        if (found == native.end()) {
            for (const auto& [source, target] : native) {
                if (reference.Nid == Nid::ComputeNid(source, reference.Library)) {
                    reference.Nid = target;
                    found = native.end();
                    break;
                }
            }
        } else {
            reference.Nid = found->second;
        }
    }
}
}
