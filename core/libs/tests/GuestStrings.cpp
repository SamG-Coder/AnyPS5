#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cmath>
#include <limits>
#include "prx/libc/include/ApplicationHeap.hpp"

extern "C" {
float APS5_VABI wcstof_nid_postfix(const std::uint16_t*, std::uint16_t**);
double APS5_VABI wcstod_nid_postfix(const std::uint16_t*, std::uint16_t**);
long double APS5_VABI wcstold_nid_postfix(const std::uint16_t*, std::uint16_t**);
std::int64_t APS5_VABI wcstol_nid_postfix(const std::uint16_t*, std::uint16_t**, int);
std::uint64_t APS5_VABI wcstoul_nid_postfix(const std::uint16_t*, std::uint16_t**, int);
std::int64_t APS5_VABI wcstoll_nid_postfix(const std::uint16_t*, std::uint16_t**, int);
std::uint64_t APS5_VABI wcstoull_nid_postfix(const std::uint16_t*, std::uint16_t**, int);
std::int64_t APS5_VABI strtol_nid_postfix(const char*, char**, int);
std::uint64_t APS5_VABI strtoul_nid_postfix(const char*, char**, int);
std::size_t APS5_VABI wcslen_nid_postfix(const std::uint16_t*);
int APS5_VABI wcscmp_nid_postfix(const std::uint16_t*, const std::uint16_t*);
int APS5_VABI wcsncmp_nid_postfix(const std::uint16_t*, const std::uint16_t*, std::size_t);
const std::uint16_t* APS5_VABI wcsstr_nid_postfix(const std::uint16_t*, const std::uint16_t*);
const std::uint16_t* APS5_VABI wmemchr_nid_postfix(const std::uint16_t*, std::uint16_t, std::size_t);
int APS5_VABI wmemcmp_nid_postfix(const std::uint16_t*, const std::uint16_t*, std::size_t);
std::uint16_t* APS5_VABI wmemcpy_nid_postfix(std::uint16_t*, const std::uint16_t*, std::size_t);
std::uint16_t* APS5_VABI wmemmove_nid_postfix(std::uint16_t*, const std::uint16_t*, std::size_t);
void* APS5_VABI aligned_alloc_nid_postfix(std::size_t, std::size_t);
int APS5_VABI asprintf_nid_postfix(char**, const char*, ...);
int APS5_VABI vasprintf_nid_postfix(char**, const char*, void*);
char* APS5_VABI strndup_nid_postfix(const char*, std::size_t);
void APS5_VABI free_nid_postfix(void*);
char* APS5_VABI basename_nid_postfix(const char*);
int* APS5_VABI __error_nid_postfix();
std::size_t APS5_VABI strnlen_nid_postfix(const char*, std::size_t);
char* APS5_VABI strncat_nid_postfix(char*, const char*, std::size_t);
char* APS5_VABI strpbrk_nid_postfix(const char*, const char*);
std::size_t APS5_VABI strcspn_nid_postfix(const char*, const char*);
std::size_t APS5_VABI strlcat_nid_postfix(char*, const char*, std::size_t);
char* APS5_VABI strtok_r_nid_postfix(char*, const char*, char**);
char* APS5_VABI strtok_nid_postfix(char*, const char*);
char* APS5_VABI strcasestr_nid_postfix(const char*, const char*);
}

static void Require(bool condition) {
    if (!condition) {
        std::fputs("Guest string check failed\n", stderr);
        std::abort();
    }
}

alignas(64) static std::array<char, 4096> allocation;
static std::size_t requestedAlignment = 0;
static std::size_t allocationSize = 0;
static unsigned frees = 0;
static bool failAllocation = false;
static void APS5_VABI UnexpectedHeapCall() { std::abort(); }
static void* APS5_VABI Allocate(std::size_t size) {
    Require(size <= allocation.size());
    allocationSize = size;
    allocation.fill('!');
    return failAllocation ? nullptr : allocation.data();
}
static void APS5_VABI Free(void* pointer) { Require(pointer == allocation.data()); ++frees; }
static void* APS5_VABI Align(std::size_t alignment, std::size_t size) {
    requestedAlignment = alignment;
    return Allocate(size);
}
static int APS5_VABI FormatAllocated(char** output, const char* format, ...) {
#ifdef _WIN32
    __builtin_sysv_va_list args;
    __builtin_sysv_va_start(args, format);
#else
    std::va_list args;
    va_start(args, format);
#endif
    const int result = vasprintf_nid_postfix(output, format, args);
#ifdef _WIN32
    __builtin_sysv_va_end(args);
#else
    va_end(args);
#endif
    return result;
}
int main() {
    const auto wideNumber = [](const char* text) {
        std::basic_string<std::uint16_t> result;
        while (*text) result += static_cast<unsigned char>(*text++);
        return result;
    };
    std::uint16_t* end = nullptr;
    auto floating = wideNumber("  -12.5tail");
    Require(wcstof_nid_postfix(floating.c_str(), &end) == -12.5f && end == floating.data() + 7);
    Require(wcstod_nid_postfix(floating.c_str(), &end) == -12.5 && end == floating.data() + 7);
    Require(wcstold_nid_postfix(floating.c_str(), &end) == -12.5L && end == floating.data() + 7);
    floating = wideNumber("0x1.8p+2!");
    Require(wcstod_nid_postfix(floating.c_str(), &end) == 6.0 && *end == '!');
    floating = wideNumber("1.000000000000000000108420217248550443400745280086994171142578125");
    Require(wcstold_nid_postfix(floating.c_str(), &end) == 1.0L + std::numeric_limits<long double>::epsilon());
    Require(*end == 0);
    floating = wideNumber("1e4000");
    Require(std::isfinite(wcstold_nid_postfix(floating.c_str(), &end)) && *end == 0);
    floating = wideNumber("1e99999!");
    Require(std::isinf(wcstof_nid_postfix(floating.c_str(), &end)) && *__error_nid_postfix() == 34 && *end == '!');
    floating = wideNumber("1e-99999!");
    Require(wcstod_nid_postfix(floating.c_str(), &end) == 0 && *__error_nid_postfix() == 34 && *end == '!');
    floating = wideNumber("-0");
    Require(std::signbit(wcstod_nid_postfix(floating.c_str(), nullptr)));
    floating = wideNumber("nan!");
    Require(std::isnan(wcstof_nid_postfix(floating.c_str(), &end)) && *end == '!');
    floating = wideNumber("+infinity!");
    Require(std::isinf(wcstod_nid_postfix(floating.c_str(), &end)) && *end == '!');
    floating = wideNumber("  invalid");
    Require(wcstod_nid_postfix(floating.c_str(), &end) == 0 && end == floating.data());
    auto number = wideNumber("  +4294967297tail");
    Require(wcstoul_nid_postfix(number.c_str(), &end, 10) == UINT64_C(4294967297));
    Require(end == number.data() + 13);
    number = wideNumber("18446744073709551615!");
    Require(wcstoull_nid_postfix(number.c_str(), &end, 10) == UINT64_MAX && end == number.data() + 20);
    number = wideNumber("18446744073709551616!");
    Require(wcstoul_nid_postfix(number.c_str(), &end, 10) == UINT64_MAX && *__error_nid_postfix() == 34);
    Require(end == number.data() + 20);
    number = wideNumber("-9223372036854775808");
    Require(wcstol_nid_postfix(number.c_str(), &end, 10) == INT64_MIN && *end == 0);
    number = wideNumber("9223372036854775808");
    Require(wcstoll_nid_postfix(number.c_str(), &end, 10) == INT64_MAX && *__error_nid_postfix() == 34);
    number = wideNumber("-1");
    Require(wcstoul_nid_postfix(number.c_str(), nullptr, 10) == UINT64_MAX);
    number = wideNumber("0x100000001!");
    Require(wcstoul_nid_postfix(number.c_str(), &end, 0) == UINT64_C(4294967297) && *end == '!');
    number = wideNumber("077!");
    Require(wcstol_nid_postfix(number.c_str(), &end, 0) == 63 && *end == '!');
    number = wideNumber("z!");
    Require(wcstol_nid_postfix(number.c_str(), &end, 36) == 35 && *end == '!');
    Require(wcstol_nid_postfix(number.c_str(), &end, 1) == 0 && *__error_nid_postfix() == 22 && end == number.data());
    number = wideNumber("  -not-a-number");
    Require(wcstol_nid_postfix(number.c_str(), &end, 10) == 0 && end == number.data());
    const std::uint16_t stopped[] = {'1', '2', 0x1234, '3', 0};
    Require(wcstol_nid_postfix(stopped, &end, 10) == 12 && end == stopped + 2);
    Require(strtol_nid_postfix("4294967297", nullptr, 10) == INT64_C(4294967297));
    Require(strtoul_nid_postfix("18446744073709551615", nullptr, 10) == UINT64_MAX);
    const std::uint16_t wide[] = {'A', 0x1234, 0xd83d, 0xde00, 'Z', 0};
    const std::uint16_t emptyWide[] = {0};
    const std::uint16_t pattern[] = {0xd83d, 0xde00, 0};
    const std::uint16_t absent[] = {'Z', 'X', 0};
    Require(wcslen_nid_postfix(wide) == 5 && wcslen_nid_postfix(emptyWide) == 0);
    Require(wcscmp_nid_postfix(wide, wide) == 0);
    Require(wcscmp_nid_postfix(wide, emptyWide) > 0);
    Require(wcsncmp_nid_postfix(wide, pattern, 0) == 0);
    Require(wcsstr_nid_postfix(wide, pattern) == wide + 2);
    Require(wcsstr_nid_postfix(wide, emptyWide) == wide);
    Require(wcsstr_nid_postfix(wide, absent) == nullptr);
    Require(wmemchr_nid_postfix(wide, 0xde00, 3) == nullptr);
    Require(wmemchr_nid_postfix(wide, 0xde00, 4) == wide + 3);
    const std::uint16_t high[] = {0xffff, 0}, low[] = {0x7fff, 0};
    Require(wmemcmp_nid_postfix(high, low, 1) > 0 && wcscmp_nid_postfix(high, low) > 0);
    std::array<std::uint16_t, 8> copied{};
    copied.fill(0xbeef);
    Require(wmemcpy_nid_postfix(copied.data() + 1, wide, 6) == copied.data() + 1);
    Require(copied.front() == 0xbeef && copied.back() == 0xbeef);
    Require(wmemcmp_nid_postfix(copied.data() + 1, wide, 6) == 0);
    Require(wmemmove_nid_postfix(copied.data() + 2, copied.data() + 1, 5) == copied.data() + 2);
    Require(wmemcmp_nid_postfix(copied.data() + 2, wide, 5) == 0);
    Require(wmemmove_nid_postfix(copied.data() + 1, copied.data() + 2, 5) == copied.data() + 1);
    Require(wmemcmp_nid_postfix(copied.data() + 1, wide, 5) == 0);
    Require(copied.front() == 0xbeef && copied.back() == 0xbeef);
    std::array<void*, 10> api{};
    api.fill(reinterpret_cast<void*>(UnexpectedHeapCall));
    api[4] = reinterpret_cast<void*>(Align);
    api[0] = reinterpret_cast<void*>(Allocate);
    api[1] = reinterpret_cast<void*>(Free);
    ApplicationHeapRegister_nid_no_patch(api.data());
    const char source[] = {'a', 'b', 'c'};
    auto* copy = strndup_nid_postfix(source, sizeof(source));
    Require(copy == allocation.data() && allocationSize == 4 && std::strcmp(copy, "abc") == 0);
    Require(allocation[4] == '!' && source[2] == 'c');
    free_nid_postfix(copy);
    copy = strndup_nid_postfix("short", 99);
    Require(allocationSize == 6 && std::strcmp(copy, "short") == 0);
    free_nid_postfix(copy);
    copy = strndup_nid_postfix(source, 0);
    Require(allocationSize == 1 && copy[0] == '\0' && allocation[1] == '!');
    free_nid_postfix(copy);
    Require(frees == 3);
    failAllocation = true;
    Require(strndup_nid_postfix("failure", 3) == nullptr && *__error_nid_postfix() == 12);
    Require(strndup_nid_postfix(nullptr, 0) == nullptr && *__error_nid_postfix() == 14);
    failAllocation = false;
    char* formatted = nullptr;
    const char expected[] = "guest:4294967297:  2.50:1,2,3,4,5,6,7";
    Require(asprintf_nid_postfix(&formatted, "%s:%ld:%*.*f:%d,%d,%d,%d,%d,%d,%d",
        "guest", INT64_C(4294967297), 6, 2, 2.5, 1, 2, 3, 4, 5, 6, 7) == sizeof(expected) - 1);
    Require(formatted == allocation.data() && allocationSize == sizeof(expected));
    Require(std::strcmp(formatted, expected) == 0);
    free_nid_postfix(formatted);
    Require(FormatAllocated(&formatted, "%02000d", 7) == 2000 && allocationSize == 2001);
    Require(formatted[1999] == '7' && formatted[2000] == '\0' && allocation[2001] == '!');
    free_nid_postfix(formatted);
    Require(asprintf_nid_postfix(&formatted, "a%cb", 0) == 3);
    Require(allocationSize == 4 && std::memcmp(formatted, "a\0b", 4) == 0);
    free_nid_postfix(formatted);
    Require(asprintf_nid_postfix(&formatted, "") == 0 && formatted && allocationSize == 1 && *formatted == '\0');
    free_nid_postfix(formatted);
    failAllocation = true;
    Require(asprintf_nid_postfix(&formatted, "%s", "failure") == -1 && formatted == nullptr);
    Require(*__error_nid_postfix() == 12 && frees == 7);
    Require(asprintf_nid_postfix(nullptr, "") == -1 && *__error_nid_postfix() == 14);
    failAllocation = false;
    for (std::size_t alignment : {1u, 2u, 4u, 8u, 16u, 32u, 64u}) {
        auto* pointer = aligned_alloc_nid_postfix(alignment, alignment * 2);
        Require(pointer == allocation.data() && requestedAlignment == alignment && allocationSize == alignment * 2);
        Require(reinterpret_cast<std::uintptr_t>(pointer) % alignment == 0);
        std::memset(pointer, 0x5a, alignment * 2);
        Require(allocation[alignment * 2] == '!');
        free_nid_postfix(pointer);
    }
    const auto previousAlignment = requestedAlignment;
    Require(aligned_alloc_nid_postfix(0, 64) == nullptr && *__error_nid_postfix() == 22);
    Require(aligned_alloc_nid_postfix(3, 6) == nullptr && *__error_nid_postfix() == 22);
    Require(aligned_alloc_nid_postfix(64, 65) == nullptr && *__error_nid_postfix() == 22);
    Require(requestedAlignment == previousAlignment);
    auto* empty = aligned_alloc_nid_postfix(64, 0);
    Require(empty == allocation.data() && allocationSize == 0);
    free_nid_postfix(empty);
    failAllocation = true;
    Require(aligned_alloc_nid_postfix(64, 128) == nullptr && *__error_nid_postfix() == 12);
    Require(frees == 15);
    Require(std::strcmp(basename_nid_postfix(nullptr), ".") == 0);
    Require(std::strcmp(basename_nid_postfix(""), ".") == 0);
    Require(std::strcmp(basename_nid_postfix("////"), "/") == 0);
    const char path[] = "/one/two///";
    Require(std::strcmp(basename_nid_postfix(path), "two") == 0);
    Require(std::strcmp(path, "/one/two///") == 0);
    Require(std::strcmp(basename_nid_postfix("one\\two"), "one\\two") == 0);
    const std::string longName(1024, 'x');
    Require(basename_nid_postfix(longName.c_str()) == nullptr && *__error_nid_postfix() == 63);
    const char bounded[] = {'a', 'b', 'c'};
    Require(strnlen_nid_postfix(bounded, 0) == 0);
    Require(strnlen_nid_postfix(bounded, sizeof(bounded)) == 3);
    Require(strnlen_nid_postfix("a", 8) == 1);
    char truncated[] = "abXX";
    Require(strlcat_nid_postfix(truncated, "cd", 2) == 4);
    Require(std::strcmp(truncated, "abXX") == 0);
    char buffer[8] = "ab";
    Require(strlcat_nid_postfix(buffer, "cdefgh", sizeof(buffer)) == 8);
    Require(std::strcmp(buffer, "abcdefg") == 0);
    Require(strlcat_nid_postfix(buffer, "xyz", 0) == 3);
    buffer[0] = '\0';
    Require(strlcat_nid_postfix(buffer, "x", 1) == 1 && buffer[0] == '\0');
    Require(strncat_nid_postfix(buffer, "xyz", 2) == buffer);
    Require(std::strcmp(buffer, "xy") == 0);
    Require(strpbrk_nid_postfix(buffer, "ay") == buffer + 1);
    Require(strpbrk_nid_postfix(buffer, "") == nullptr);
    Require(strcspn_nid_postfix(buffer, "y") == 1);
    char first[] = ",a,,b,";
    char second[] = "x:y";
    char* firstState = nullptr;
    char* secondState = nullptr;
    Require(std::strcmp(strtok_r_nid_postfix(first, ",", &firstState), "a") == 0);
    Require(std::strcmp(strtok_r_nid_postfix(second, ":", &secondState), "x") == 0);
    Require(std::strcmp(strtok_r_nid_postfix(nullptr, ",", &firstState), "b") == 0);
    Require(strtok_r_nid_postfix(nullptr, ",", &firstState) == nullptr);
    Require(strtok_r_nid_postfix(nullptr, ",", &firstState) == nullptr);
    Require(std::strcmp(strtok_r_nid_postfix(nullptr, "", &secondState), "y") == 0);
    char hostTokens[] = "host:next";
    Require(std::strcmp(std::strtok(hostTokens, ":"), "host") == 0);
    char guestTokens[] = ",one,,two:three";
    Require(std::strcmp(strtok_nid_postfix(guestTokens, ","), "one") == 0);
    Require(std::strcmp(strtok_nid_postfix(nullptr, ":,"), "two") == 0);
    Require(std::strcmp(strtok_nid_postfix(nullptr, ""), "three") == 0);
    Require(strtok_nid_postfix(nullptr, ",") == nullptr);
    Require(strtok_nid_postfix(nullptr, ",") == nullptr);
    Require(std::strcmp(std::strtok(nullptr, ":"), "next") == 0);
    const char text[] = "aABAbC";
    Require(strcasestr_nid_postfix(text, "ababc") == text + 1);
    Require(strcasestr_nid_postfix(text, "") == text);
    Require(strcasestr_nid_postfix(text, "abcdef") == nullptr);
    Require(strcasestr_nid_postfix("", "a") == nullptr);
    const char highBytes[] = {static_cast<char>(0xff), 'A', 0};
    Require(strcasestr_nid_postfix(highBytes, "a") == highBytes + 1);
}
