#include <relinker/analysis/AgcImportLowering.hpp>
#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <unordered_map>
#include <nid/NidCompute.hpp>
#include <relinker/analysis/ImportLibraries.hpp>
namespace Relinker {
static const std::unordered_map<std::string, std::string>& nativeMap() {
    static const std::unordered_map<std::string, std::string> native = {

        {"sceAgcInit", "aps5NativeAgcInit"},
        {"sceAgcCbReleaseMem", "aps5NativeAgcReleaseMem"},
        {"sceAgcSuspendPoint", "aps5NativeAgcSuspendPoint"},
        {"sceAgcLinkShaders", "aps5NativeAgcLinkShaders"},
        {"sceAgcGetRegisterDefaults", "aps5NativeAgcGetRegisterDefaults"},
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
    return native;
}
std::optional<std::string> AgcImportLowering::NativeSymbol(const std::string& imported, const std::string& library) {
    const std::string name(ImportLibraries::SymbolName(imported));
    const auto& native=nativeMap();
    if (const auto found=native.find(name); found!=native.end()) return found->second;
    for (const auto& [source,target]:native) if (name==Nid::ComputeNid(source,library)) return target;
    return std::nullopt;
}
void AgcImportLowering::Apply(std::vector<NidReference>& references) {
    for (auto& reference : references) {
        if (!AgcLoweringAnalyzer::IsAgcLibrary(reference.Library)) continue;
        if (const auto native=NativeSymbol(reference.Nid,reference.Library)) reference.Nid=*native;
    }
}
}
