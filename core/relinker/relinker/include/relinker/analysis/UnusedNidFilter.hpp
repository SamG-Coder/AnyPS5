#ifndef RELINKER_ANALYSIS_UNUSEDNIDFILTER_HPP
#define RELINKER_ANALYSIS_UNUSEDNIDFILTER_HPP

#include <relinker/domain/IUnusedNidFilter.hpp>
#include <memory>
#include <relinker/lowering/NativeFunctions.hpp>

namespace Relinker {

std::shared_ptr<IUnusedNidFilter> MakeUnusedNidFilter();
std::shared_ptr<IUnusedNidFilter> MakeStrictUnusedNidFilter(std::vector<NativeFunctionBinding> bindings = {});

}

#endif
