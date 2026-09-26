#include <relinker/analysis/ImportLibraries.hpp>
#include <limits>

namespace Relinker {
namespace {
std::string readString(std::span<const std::uint8_t> bytes, std::uint32_t offset) {
    if (offset >= bytes.size()) throw RelinkerException("Import library string offset is out of bounds", offset);
    std::size_t end = offset;
    while (end < bytes.size() && bytes[end] != 0) ++end;
    if (end == bytes.size() || end == offset) throw RelinkerException("Invalid import library name", offset);
    return {reinterpret_cast<const char*>(bytes.data() + offset), end - offset};
}
}

ImportLibraries::ImportLibraries(std::span<const DynamicTag> tags, std::span<const std::uint8_t> strings) {
    for (const auto& tag : tags) {
        auto* table = tag.Tag == 0x61000015 || tag.Tag == 0x61000049 ? &libraries
                    : tag.Tag == 0x6100000f || tag.Tag == 0x61000045 ? &modules : nullptr;
        if (table == nullptr) continue;
        const auto id = static_cast<std::uint16_t>(tag.Value >> 48u);
        const auto name = readString(strings, static_cast<std::uint32_t>(tag.Value));
        const auto [it, inserted] = table->emplace(id, name);
        if (!inserted && it->second != name) throw RelinkerException("Conflicting import library/module identifiers");
    }
}

std::uint16_t ImportLibraries::DecodeId(std::string_view value) {
    constexpr std::string_view alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
    if (value.empty()) throw RelinkerException("Empty import library/module identifier");
    std::uint32_t decoded = 0;
    for (const char ch : value) {
        const auto digit = alphabet.find(ch);
        if (digit == std::string_view::npos || decoded > (std::numeric_limits<std::uint16_t>::max() - digit) / 64u)
            throw RelinkerException("Invalid import library/module identifier");
        decoded = decoded * 64u + static_cast<std::uint32_t>(digit);
    }
    return static_cast<std::uint16_t>(decoded);
}

std::string_view ImportLibraries::SymbolName(std::string_view imported) {
    return imported.substr(0, imported.find('#'));
}

std::string ImportLibraries::LibraryFor(std::string_view imported) const {
    const auto first = imported.find('#');
    if (first == std::string_view::npos) return {};
    const auto second = imported.find('#', first + 1);
    if (first == 0 || second == std::string_view::npos || imported.find('#', second + 1) != std::string_view::npos)
        throw RelinkerException("Malformed library-qualified import: " + std::string(imported));
    const auto libraryId = DecodeId(imported.substr(first + 1, second - first - 1));
    const auto moduleId = DecodeId(imported.substr(second + 1));
    const auto library = libraries.find(libraryId);
    if (library == libraries.end() || !modules.contains(moduleId))
        throw RelinkerException("Unresolved import library/module identifier: " + std::string(imported));
    return library->second;
}
}
