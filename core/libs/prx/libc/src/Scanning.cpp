#include "prx/libc/include/WindowsFormatting.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdarg>

extern "C" int APS5_VABI vsscanf_nid_postfix(const char* input, const char* format, const void* source) {
    if (!input || !format || !source) throw std::runtime_error("vsscanf: null argument");
    static_assert(sizeof(long long) == 8);
    static_assert(sizeof(long double) == 16 && std::numeric_limits<long double>::digits == 64);
    LibcDetail::FormatArguments args(source);
    std::vector<void*> destinations;
    std::string nativeFormat;
    while (*format) {
        if (*format != '%') { nativeFormat += *format++; continue; }
        nativeFormat += *format++;
        if (*format == '%') { nativeFormat += *format++; continue; }
        const bool suppressed = *format == '*';
        if (suppressed) nativeFormat += *format++;
        unsigned width = 0;
        bool hasWidth = false;
        while (*format >= '0' && *format <= '9') {
            hasWidth = true;
            const unsigned digit = *format - '0';
            if (width > (INT_MAX - digit) / 10) throw std::runtime_error("vsscanf: field width overflow");
            width = width * 10 + digit;
            nativeFormat += *format++;
        }
        if (hasWidth && !width) throw std::runtime_error("vsscanf: zero field width");
        std::string length;
        if (*format && std::strchr("hljztL", *format)) {
            length += *format++;
            if ((length == "h" && *format == 'h') || (length == "l" && *format == 'l')) length += *format++;
        }
        const char conversion = *format;
        if (!conversion) throw std::runtime_error("vsscanf: incomplete conversion");
        ++format;
        if (std::strchr("diouxXn", conversion)) {
            if (length == "L") throw std::runtime_error("vsscanf: invalid integer length");
            if (conversion == 'n' && (hasWidth || suppressed)) throw std::runtime_error("vsscanf: invalid count conversion");
            if (length == "l" || length == "j" || length == "z" || length == "t") length = "ll";
        } else if (std::strchr("aAeEfFgG", conversion)) {
            if (!length.empty() && length != "l" && length != "L") throw std::runtime_error("vsscanf: invalid floating length");
        } else if (std::strchr("cs[", conversion)) {
            if (!length.empty() && length != "l") throw std::runtime_error("vsscanf: invalid character length");
            if (length == "l" && sizeof(wchar_t) != 2) throw std::runtime_error("vsscanf: host wide character width is unsupported");
        } else if (conversion != 'p' || !length.empty()) {
            throw std::runtime_error("vsscanf: unsupported conversion");
        }
        nativeFormat += length;
        nativeFormat += conversion;
        if (conversion == '[') {
            if (*format == '^') nativeFormat += *format++;
            if (*format == ']') nativeFormat += *format++;
            while (*format && *format != ']') nativeFormat += *format++;
            if (!*format) throw std::runtime_error("vsscanf: unterminated scan set");
            nativeFormat += *format++;
        }
        if (!suppressed) {
            auto* destination = args.Next<void*>();
            if (!destination) throw std::runtime_error("vsscanf: null destination");
            destinations.push_back(destination);
        }
    }
    destinations.push_back(nullptr);
#ifdef _WIN32
    static_assert(std::is_same_v<std::va_list, char*>);
    return std::vsscanf(input, nativeFormat.c_str(), reinterpret_cast<char*>(destinations.data()));
#else
    VaList native{};
    native.gp_offset = 48;
    native.fp_offset = 176;
    native.overflow_arg_area = destinations.data();
    return std::vsscanf(input, nativeFormat.c_str(), *reinterpret_cast<std::va_list*>(&native));
#endif
}

extern "C" int APS5_VABI sscanf_nid_postfix(const char* input, const char* format, ...) {
#ifdef _WIN32
    __builtin_sysv_va_list args;
    __builtin_sysv_va_start(args, format);
#else
    std::va_list args;
    va_start(args, format);
#endif
    const int result = vsscanf_nid_postfix(input, format, args);
#ifdef _WIN32
    __builtin_sysv_va_end(args);
#else
    va_end(args);
#endif
    return result;
}
