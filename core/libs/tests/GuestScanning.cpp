#include "prx/libc/include/general/VabiMacros.hpp"
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <source_location>
#include <limits>

extern "C" int APS5_VABI sscanf_nid_postfix(const char*, const char*, ...);
extern "C" int APS5_VABI vsscanf_nid_postfix(const char*, const char*, const void*);
static void Require(bool value, const std::source_location location = std::source_location::current()) {
    if (!value) {
        std::fprintf(stderr, "Guest scanning check failed at line %u\n", location.line());
        std::abort();
    }
}
static int APS5_VABI Scan(const char* input, const char* format, ...) {
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
int main() {
    struct Guarded { std::int64_t value; std::uint64_t guard; } signedValue{0, UINT64_MAX};
    std::uint64_t unsignedValue = 0;
    float single = 0;
    double decimal = 0;
    long double extended = 0;
    char word[8]{};
    int count = -1;
    Require(Scan("-4294967297 18446744073709551615 1.25 -2.5 1.125 hello!", "%ld %zu %f %lf %Lf %5s%n",
        &signedValue.value, &unsignedValue, &single, &decimal, &extended, word, &count) == 6);
    Require(signedValue.value == -4294967297LL && signedValue.guard == UINT64_MAX);
    Require(unsignedValue == UINT64_MAX && single == 1.25f && decimal == -2.5 && extended == 1.125L);
    Require(std::strcmp(word, "hello") == 0 && count == 54);
    Require(Scan("1.000000000000000000108420217248550443400745280086994171142578125", "%Lf", &extended) == 1);
    Require(extended == 1.0L + std::numeric_limits<long double>::epsilon());
    for (const auto* format : {"%ld", "%lld", "%jd", "%td"}) {
        signedValue.value = 0;
        Require(Scan("9223372036854775807", format, &signedValue.value) == 1);
        Require(signedValue.value == INT64_MAX && signedValue.guard == UINT64_MAX);
    }
    std::array<int, 10> values{};
    Require(sscanf_nid_postfix("1 2 3 4 5 6 7 8 9 10", "%d%d%d%d%d%d%d%d%d%d",
        &values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7], &values[8], &values[9]) == 10);
    for (int index = 0; index < 10; ++index) Require(values[index] == index + 1);
    signed char small = 0;
    unsigned short shortValue = 0;
    Require(Scan("skip -12 65535 0xff", "%*s %hhd %hu %i", &small, &shortValue, &values[0]) == 3);
    Require(small == -12 && shortValue == 65535 && values[0] == 255);
    std::array<char, 8> chars;
    chars.fill('!');
    Require(Scan("abc123", "%3[a-z]%d", chars.data(), &values[0]) == 2);
    Require(std::strcmp(chars.data(), "abc") == 0 && chars[4] == '!' && values[0] == 123);
    Require(Scan("]-rest", "%2[]-]", chars.data()) == 1 && std::strcmp(chars.data(), "]-") == 0);
    Require(Scan("abc:rest", "%3[^:]", chars.data()) == 1 && std::strcmp(chars.data(), "abc") == 0);
    chars.fill('!');
    Require(Scan(" a", "%2c", chars.data()) == 1 && chars[0] == ' ' && chars[1] == 'a' && chars[2] == '!');
    std::int64_t wideCount = -1;
    Require(Scan("  % 27", " %% %d%ln", &values[0], &wideCount) == 1 && values[0] == 27 && wideCount == 6);
    values[0] = 99;
    Require(Scan("", "%d", &values[0]) == EOF && values[0] == 99);
    Require(Scan("x", "%d", &values[0]) == 0 && values[0] == 99);
    Require(Scan("1 x", "%d%d", &values[0], &values[1]) == 1 && values[1] == 2);
    Require(Scan("", "") == 0);
    Require(Scan("42", "%*d") == 0);
    Require(Scan("0x1.8p+2", "%la", &decimal) == 1 && decimal == 6.0);
    void* pointer = reinterpret_cast<void*>(std::uintptr_t{0x1234});
    char pointerText[64]{};
    std::snprintf(pointerText, sizeof(pointerText), "%p", pointer);
    void* parsed = nullptr;
    Require(Scan(pointerText, "%p", &parsed) == 1 && parsed == pointer);
#ifdef _WIN32
    std::array<std::uint16_t, 5> wide{};
    wide.fill(0xbeef);
    Require(Scan("abc", "%3ls", wide.data()) == 1);
    Require(wide[0] == 'a' && wide[1] == 'b' && wide[2] == 'c' && wide[3] == 0 && wide[4] == 0xbeef);
#endif
    for (const auto* format : {"%", "%[abc", "%1$d", "%ms", "%Ld"}) {
        bool rejected = false;
        try { Scan("123", format, &values[0]); }
        catch (const std::runtime_error&) { rejected = true; }
        Require(rejected);
    }
}
