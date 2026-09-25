#include "prx/libc/include/WindowsFormatting.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdarg>

namespace {
using Wide = std::uint16_t;
class Output {
    Wide* buffer;
    std::size_t capacity;
    std::size_t count = 0;
public:
    Output(Wide* destination, std::size_t size) : buffer(destination), capacity(size) {
        if (size && !destination) throw std::runtime_error("swprintf: null destination");
        if (size) buffer[0] = 0;
    }
    void Put(Wide value) {
        if (count == INT_MAX) throw std::overflow_error("swprintf: output too long");
        if (capacity && count < capacity - 1) { buffer[count] = value; buffer[count + 1] = 0; }
        ++count;
    }
    void Pad(std::size_t size) { while (size--) Put(' '); }
    template<typename T> void Number(const std::string& format, T value) {
        std::string text;
        LibcDetail::FormatOutput native(nullptr, 0, &text);
        native.Value(format, value);
        for (const unsigned char character : text) {
            if (character > 127) throw std::runtime_error("swprintf: non-ASCII numeric locale is unsupported");
            Put(character);
        }
    }
    int Count() const { return static_cast<int>(count); }
    int Result() const { return count < capacity ? static_cast<int>(count) : -1; }
};
int ReadDecimal(const Wide*& cursor) {
    int value = 0;
    while (*cursor >= '0' && *cursor <= '9') {
        const int digit = *cursor++ - '0';
        if (value > (INT_MAX - digit) / 10) throw std::overflow_error("swprintf: width or precision overflow");
        value = value * 10 + digit;
    }
    return value;
}
}

extern "C" int APS5_VABI vswprintf_nid_postfix(Wide* destination, std::size_t size, const Wide* format, const void* source) {
    if (!format || !source) throw std::runtime_error("swprintf: null format or arguments");
    LibcDetail::FormatArguments args(source);
    Output output(destination, size);
    while (*format) {
        if (*format != '%') { output.Put(*format++); continue; }
        ++format;
        if (*format == '%') { output.Put(*format++); continue; }
        std::string spec = "%";
        bool left = false;
        while (*format && *format < 128 && std::strchr("-+ #0", static_cast<char>(*format))) {
            left |= *format == '-';
            spec += static_cast<char>(*format++);
        }
        int width = 0;
        if (*format == '*') {
            ++format;
            width = args.Next<int>();
            if (width == INT_MIN) throw std::overflow_error("swprintf: width overflow");
            if (width < 0) { left = true; spec += '-'; width = -width; }
        } else width = ReadDecimal(format);
        if (width) spec += std::to_string(width);
        int precision = -1;
        if (*format == '.') {
            ++format;
            if (*format == '*') { ++format; precision = args.Next<int>(); }
            else precision = ReadDecimal(format);
            if (precision >= 0) spec += "." + std::to_string(precision);
        }
        std::string length;
        if (*format && *format < 128 && std::strchr("hljztL", static_cast<char>(*format))) {
            length += static_cast<char>(*format++);
            if ((length == "h" && *format == 'h') || (length == "l" && *format == 'l'))
                length += static_cast<char>(*format++);
        }
        if (!*format || *format > 127) throw std::runtime_error("swprintf: invalid conversion");
        const char conversion = static_cast<char>(*format++);
        const bool integer = length.empty() || length == "h" || length == "hh" || length == "l" ||
            length == "ll" || length == "j" || length == "z" || length == "t";
        if ((conversion == 'd' || conversion == 'i') && integer) {
            long long value;
            if (length.empty() || length == "h" || length == "hh") {
                value = args.Next<int>();
                if (length == "h") value = static_cast<short>(value);
                if (length == "hh") value = static_cast<signed char>(value);
            } else value = args.Next<long long>();
            output.Number(spec + "ll" + conversion, value);
        } else if (std::strchr("ouxX", conversion) && integer) {
            unsigned long long value;
            if (length.empty() || length == "h" || length == "hh") {
                value = args.Next<unsigned int>();
                if (length == "h") value = static_cast<unsigned short>(value);
                if (length == "hh") value = static_cast<unsigned char>(value);
            } else value = args.Next<unsigned long long>();
            output.Number(spec + "ll" + conversion, value);
        } else if (std::strchr("aAeEfFgG", conversion) && (length.empty() || length == "l" || length == "L")) {
            if (length == "L") output.Number(spec + "L" + conversion, args.Next<long double>());
            else output.Number(spec + conversion, args.Next<double>());
        } else if (conversion == 'p' && length.empty()) {
            output.Number(spec + 'p', args.Next<void*>());
        } else if ((conversion == 's' || conversion == 'c') && (length.empty() || length == "l")) {
            std::basic_string<Wide> text;
            if (conversion == 'c') {
                const auto value = args.Next<unsigned int>();
                if ((length.empty() && value > 127) || value > 65535) { errno = 86; return -1; }
                text += static_cast<Wide>(value);
            } else if (length == "l") {
                const auto* value = args.Next<const Wide*>();
                if (!value) throw std::runtime_error("swprintf: null string");
                while ((precision < 0 || text.size() < static_cast<std::size_t>(precision)) && *value) text += *value++;
            } else {
                const auto* value = args.Next<const unsigned char*>();
                if (!value) throw std::runtime_error("swprintf: null string");
                while ((precision < 0 || text.size() < static_cast<std::size_t>(precision)) && *value) {
                    if (*value > 127) { errno = 86; return -1; }
                    text += *value++;
                }
            }
            const auto padding = static_cast<std::size_t>(width) > text.size() ? width - text.size() : 0;
            if (!left) output.Pad(padding);
            for (const auto value : text) output.Put(value);
            if (left) output.Pad(padding);
        } else if (conversion == 'n' && integer && spec == "%") {
            void* value = args.Next<void*>();
            if (!value) throw std::runtime_error("swprintf: null count pointer");
            if (length == "hh") *static_cast<signed char*>(value) = static_cast<signed char>(output.Count());
            else if (length == "h") *static_cast<short*>(value) = static_cast<short>(output.Count());
            else if (length.empty()) *static_cast<int*>(value) = output.Count();
            else *static_cast<std::int64_t*>(value) = output.Count();
        } else throw std::runtime_error("swprintf: unsupported conversion");
    }
    return output.Result();
}

extern "C" int APS5_VABI swprintf_nid_postfix(Wide* destination, std::size_t size, const Wide* format, ...) {
#ifdef _WIN32
    __builtin_sysv_va_list args;
    __builtin_sysv_va_start(args, format);
#else
    std::va_list args;
    va_start(args, format);
#endif
    const int result = vswprintf_nid_postfix(destination, size, format, args);
#ifdef _WIN32
    __builtin_sysv_va_end(args);
#else
    va_end(args);
#endif
    return result;
}
