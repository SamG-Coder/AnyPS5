#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <stdexcept>

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
