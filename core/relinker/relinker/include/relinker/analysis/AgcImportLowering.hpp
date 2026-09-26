#ifndef RELINKER_ANALYSIS_AGCIMPORTLOWERING_HPP
#define RELINKER_ANALYSIS_AGCIMPORTLOWERING_HPP
#include <relinker/domain/Types.hpp>
#include <vector>
#include <optional>
#include <string>
namespace Relinker {
class AgcImportLowering {
public:
    static void Apply(std::vector<NidReference>& references);
    static std::optional<std::string> NativeSymbol(const std::string& imported, const std::string& library);
};
}
#endif
