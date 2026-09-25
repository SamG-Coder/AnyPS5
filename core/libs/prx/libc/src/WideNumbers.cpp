#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cwchar>
#include <string>
#include <stdexcept>
#include <cerrno>
#include <limits>

namespace {
std::wstring HostText(const std::uint16_t* input) {
    if (!input) throw std::runtime_error("Wide numeric conversion: null input");
    std::wstring text;
    for (const auto* cursor = input; *cursor; ++cursor) text.push_back(static_cast<wchar_t>(*cursor));
    return text;
}
template<typename TResult, typename TConvert>
TResult Convert(const std::uint16_t* input, std::uint16_t** end, int base, TConvert convert) {
    if (!input) throw std::runtime_error("Wide integer conversion: null input");
    if (end) *end = const_cast<std::uint16_t*>(input);
    if (base != 0 && (base < 2 || base > 36)) { errno = 22; return 0; }
    const auto text = HostText(input);
    wchar_t* position = nullptr;
    const auto value = convert(text.c_str(), &position, base);
    if (end) *end = const_cast<std::uint16_t*>(input) + (position - text.c_str());
    return static_cast<TResult>(value);
}
template<typename TConvert>
auto ConvertFloating(const std::uint16_t* input, std::uint16_t** end, TConvert convert) {
    const auto text = HostText(input);
    wchar_t* position = nullptr;
    const auto result = convert(text.c_str(), &position);
    if (end) *end = const_cast<std::uint16_t*>(input) + (position - text.c_str());
    return result;
}
}
extern "C" {
float APS5_VABI wcstof_nid_postfix(const std::uint16_t* input, std::uint16_t** end) {
    return ConvertFloating(input, end, std::wcstof);
}
double APS5_VABI wcstod_nid_postfix(const std::uint16_t* input, std::uint16_t** end) {
    return ConvertFloating(input, end, std::wcstod);
}
long double APS5_VABI wcstold_nid_postfix(const std::uint16_t* input, std::uint16_t** end) {
    static_assert(sizeof(long double) == 16 && std::numeric_limits<long double>::digits == 64 &&
        std::numeric_limits<long double>::max_exponent == 16384);
    return ConvertFloating(input, end, std::wcstold);
}
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
