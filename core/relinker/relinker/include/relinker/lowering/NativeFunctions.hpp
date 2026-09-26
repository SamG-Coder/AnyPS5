#ifndef RELINKER_LOWERING_NATIVEFUNCTIONS_HPP
#define RELINKER_LOWERING_NATIVEFUNCTIONS_HPP

#include <relinker/domain/RelinkResult.hpp>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace Relinker {

struct NativeFunctionBinding {
    VirtualAddress address;
    std::string symbol;
    std::string library;
    std::vector<std::uint8_t> expected;
};

// A manifest describes audited ABI-compatible native replacements. This is an
// offline machine-code transformation, not an instruction interpreter or JIT.
std::vector<NativeFunctionBinding> ReadNativeFunctionBindings(const std::filesystem::path& path);
void LowerNativeFunctions(std::vector<std::uint8_t>& source, RelinkResult& result,
                          std::span<const NativeFunctionBinding> bindings);

}
#endif
