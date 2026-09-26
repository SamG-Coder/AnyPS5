#ifndef RELINKER_ANALYSIS_IMPORTLIBRARIES_HPP
#define RELINKER_ANALYSIS_IMPORTLIBRARIES_HPP

#include <relinker/domain/Types.hpp>
#include <map>
#include <span>
#include <string>
#include <string_view>

namespace Relinker {

// Offline ELF metadata, not a runtime registry. Symbol suffixes identify the
// import library/module independently of the native DT_NEEDED search order.
class ImportLibraries {
public:
    ImportLibraries(std::span<const DynamicTag> tags, std::span<const std::uint8_t> strings);
    std::string LibraryFor(std::string_view imported) const;
    static std::string_view SymbolName(std::string_view imported);

private:
    std::map<std::uint16_t, std::string> libraries;
    std::map<std::uint16_t, std::string> modules;
    static std::uint16_t DecodeId(std::string_view value);
};

}
#endif
