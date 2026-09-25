#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cwchar>
#include <string>
#include <stdexcept>
#include <cerrno>

namespace {
template<typename TResult, typename TConvert>
TResult Convert(const std::uint16_t* input, std::uint16_t** end, int base, TConvert convert) {
    if (!input) throw std::runtime_error("Wide integer conversion: null input");
    if (end) *end = const_cast<std::uint16_t*>(input);
    if (base != 0 && (base < 2 || base > 36)) { errno = 22; return 0; }
    std::wstring text;
    for (const auto* cursor = input; *cursor; ++cursor) text.push_back(static_cast<wchar_t>(*cursor));
    wchar_t* position = nullptr;
    const auto value = convert(text.c_str(), &position, base);
    if (end) *end = const_cast<std::uint16_t*>(input) + (position - text.c_str());
    return static_cast<TResult>(value);
}
}
extern "C" {
std::int64_t APS5_VABI wcstol_nid_postfix(const std::uint16_t* input, std::uint16_t** end, int base) {
    return Convert<std::int64_t>(input, end, base, std::wcstoll);
}
std::uint64_t APS5_VABI wcstoul_nid_postfix(const std::uint16_t* input, std::uint16_t** end, int base) {
    return Convert<std::uint64_t>(input, end, base, std::wcstoull);
}
std::int64_t APS5_VABI wcstoll_nid_postfix(const std::uint16_t* input, std::uint16_t** end, int base) {
    return Convert<std::int64_t>(input, end, base, std::wcstoll);
}
std::uint64_t APS5_VABI wcstoull_nid_postfix(const std::uint16_t* input, std::uint16_t** end, int base) {
    return Convert<std::uint64_t>(input, end, base, std::wcstoull);
}
}
