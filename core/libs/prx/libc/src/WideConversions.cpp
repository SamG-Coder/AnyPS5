#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <stdexcept>
#include <cwchar>
#include <cstring>
#include <climits>

namespace {
struct alignas(8) ConversionState { unsigned char bytes[128]{}; };
static_assert(sizeof(std::mbstate_t) <= sizeof(ConversionState));
std::mbstate_t ReadState(const ConversionState* state) {
    std::mbstate_t native{};
    std::memcpy(&native, state->bytes, sizeof(native));
    return native;
}
void WriteState(ConversionState* state, const std::mbstate_t& native) {
    std::memcpy(state->bytes, &native, sizeof(native));
}
}

extern "C" std::size_t APS5_VABI mbrtowc_nid_postfix(std::uint16_t* output, const char* input,
    std::size_t size, ConversionState* state) {
    if (input && !size) return static_cast<std::size_t>(-2);
    static thread_local ConversionState internal;
    if (!state) state = &internal;
    auto native = ReadState(state);
    wchar_t value = 0;
    const auto result = std::mbrtowc(&value, input ? input : "", input ? size : 1, &native);
    WriteState(state, native);
    if (result == static_cast<std::size_t>(-1)) { errno = 86; return result; }
    if (result == static_cast<std::size_t>(-2)) return result;
    if (static_cast<std::uint32_t>(value) > UINT16_MAX) { errno = 86; return static_cast<std::size_t>(-1); }
    if (input && output) *output = static_cast<std::uint16_t>(value);
    return result;
}

extern "C" std::size_t APS5_VABI wcrtomb_nid_postfix(char* output, std::uint16_t input, ConversionState* state) {
    static thread_local ConversionState internal;
    if (!state) state = &internal;
    auto native = ReadState(state);
    char reset[MB_LEN_MAX]{};
    const auto result = std::wcrtomb(output ? output : reset, output ? static_cast<wchar_t>(input) : L'\0', &native);
    WriteState(state, native);
    if (result == static_cast<std::size_t>(-1)) errno = 86;
    return result;
}

extern "C" int APS5_VABI mbsinit_nid_postfix(const ConversionState* state) {
    if (!state) return 1;
    const auto native = ReadState(state);
    return std::mbsinit(&native);
}

extern "C" std::size_t APS5_VABI wcstombs_nid_postfix(char* output, const std::uint16_t* input, std::size_t capacity) {
    if (!input) throw std::runtime_error("wcstombs: null input");
    if (output && !capacity) return 0;
    std::wstring text;
    while ((!output || text.size() < capacity) && input[text.size()])
        text.push_back(static_cast<wchar_t>(input[text.size()]));
    const auto result = std::wcstombs(output, text.c_str(), capacity);
    if (result == static_cast<std::size_t>(-1)) errno = 86;
    return result;
}
