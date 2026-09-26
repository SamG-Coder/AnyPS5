#ifndef RELINKER_ANALYSIS_AGCIMPORTLOWERING_HPP
#define RELINKER_ANALYSIS_AGCIMPORTLOWERING_HPP
#include <relinker/domain/Types.hpp>
#include <vector>
namespace Relinker {
class AgcImportLowering {
public:
    static void Apply(std::vector<NidReference>& references);
};
}
#endif
