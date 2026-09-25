#include "prx/libc/include/ApplicationHeap.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cwchar>
#include <cstdio>

#include "prx/libc/include/General.hpp"

extern "C" {

void* APS5_VABI memset_nid_postfix(void* s, int c, size_t n) {
    // APS5_LOG_OUT("s=%p c=%d n=%zu", s, c, n);
    return std::memset(s, c, n);
}

void* APS5_VABI memcpy_nid_postfix(void* dest, const void* src, size_t n) {
    return std::memcpy(dest, src, n);
}

void* APS5_VABI memmove_nid_postfix(void* dest, const void* src, size_t n) {
    return std::memmove(dest, src, n);
}

int APS5_VABI memcmp_nid_postfix(const void* s1, const void* s2, size_t n) {
    return std::memcmp(s1, s2, n);
}

const void* APS5_VABI memchr_nid_postfix(const void* s, int c, size_t n) {
    return std::memchr(s, c, n);
}

int APS5_VABI strcmp_nid_postfix(const char* s1, const char* s2) {
    return std::strcmp(s1, s2);
}

int APS5_VABI strncmp_nid_postfix(const char* s1, const char* s2, size_t n) {
    return std::strncmp(s1, s2, n);
}

size_t APS5_VABI strlen_nid_postfix(const char* s) {
    return std::strlen(s);
}

char* APS5_VABI strcpy_nid_postfix(char* dest, const char* src) {
    return std::strcpy(dest, src);
}

char* APS5_VABI strncpy_nid_postfix(char* dest, const char* src, size_t count) {
    return std::strncpy(dest, src, count);
}

char* APS5_VABI strcat_nid_postfix(char* dest, const char* src) {
    return std::strcat(dest, src);
}

const char* APS5_VABI strchr_nid_postfix(const char* s, int c) {
    return std::strchr(s, c);
}

char* APS5_VABI strrchr_nid_postfix(const char* s, int c) {
    return std::strrchr(const_cast<char*>(s), c);
}

char* APS5_VABI strstr_nid_postfix(const char* haystack, const char* needle) {
    return std::strstr(const_cast<char*>(haystack), needle);
}

size_t APS5_VABI strlcpy_nid_postfix(char* dest, const char* src, size_t size) {
    const size_t srcLen = std::strlen(src);
    if (size != 0u) {
        const size_t copyLen = srcLen < size - 1u ? srcLen : size - 1u;
        std::memcpy(dest, src, copyLen);
        dest[copyLen] = '\0';
    }
    return srcLen;
}

std::int64_t APS5_VABI strtol_nid_postfix(const char* str, char** endptr, int base) {
    return std::strtoll(str, endptr, base);
}

std::uint64_t APS5_VABI strtoul_nid_postfix(const char* str, char** endptr, int base) {
    return std::strtoull(str, endptr, base);
}

long long APS5_VABI strtoll_nid_postfix(const char* str, char** endptr, int base) {
    return std::strtoll(str, endptr, base);
}

unsigned long long APS5_VABI strtoull_nid_postfix(const char* str, char** endptr, int base) {
    return std::strtoull(str, endptr, base);
}

double APS5_VABI strtod_nid_postfix(const char* str, char** endptr) {
    return std::strtod(str, endptr);
}

double APS5_VABI atof_nid_postfix(const char* str) { return std::atof(str); }
float APS5_VABI strtof_nid_postfix(const char* str, char** endptr) { return std::strtof(str, endptr); }
long double APS5_VABI strtold_nid_postfix(const char* str, char** endptr) {
    static_assert(sizeof(long double) == 16, "Guest long double requires x87 extended precision storage");
    return std::strtold(str, endptr);
}

int APS5_VABI atoi_nid_postfix(const char* str) {
    return std::atoi(str);
}

size_t APS5_VABI wcslen_nid_postfix(const std::uint16_t* text) {
    size_t length = 0;
    while (text[length]) ++length;
    return length;
}

int APS5_VABI wcscmp_nid_postfix(const std::uint16_t* first, const std::uint16_t* second) {
    while (*first && *first == *second) { ++first; ++second; }
    return (*first > *second) - (*first < *second);
}

int APS5_VABI wcsncmp_nid_postfix(const std::uint16_t* first, const std::uint16_t* second, size_t count) {
    for (size_t index = 0; index < count; ++index) {
        if (first[index] != second[index]) return first[index] > second[index] ? 1 : -1;
        if (!first[index]) break;
    }
    return 0;
}

const std::uint16_t* APS5_VABI wcsstr_nid_postfix(const std::uint16_t* text, const std::uint16_t* pattern) {
    if (!*pattern) return text;
    for (; *text; ++text) {
        size_t index = 0;
        while (pattern[index] && text[index] == pattern[index]) ++index;
        if (!pattern[index]) return text;
    }
    return nullptr;
}

const std::uint16_t* APS5_VABI wmemchr_nid_postfix(const std::uint16_t* text, std::uint16_t value, size_t count) {
    for (size_t index = 0; index < count; ++index) if (text[index] == value) return text + index;
    return nullptr;
}

int APS5_VABI wmemcmp_nid_postfix(const std::uint16_t* first, const std::uint16_t* second, size_t count) {
    for (size_t index = 0; index < count; ++index)
        if (first[index] != second[index]) return first[index] > second[index] ? 1 : -1;
    return 0;
}

std::uint16_t* APS5_VABI wmemcpy_nid_postfix(std::uint16_t* destination, const std::uint16_t* source, size_t count) {
    for (size_t index = 0; index < count; ++index) destination[index] = source[index];
    return destination;
}

std::uint16_t* APS5_VABI wmemmove_nid_postfix(std::uint16_t* destination, const std::uint16_t* source, size_t count) {
    if (reinterpret_cast<std::uintptr_t>(destination) > reinterpret_cast<std::uintptr_t>(source)) {
        for (size_t index = count; index > 0; --index) destination[index - 1] = source[index - 1];
    } else {
        for (size_t index = 0; index < count; ++index) destination[index] = source[index];
    }
    return destination;
}

}


extern "C" {

int APS5_VABI strcasecmp_nid_postfix(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        unsigned char a = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(*s1)));
        unsigned char b = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(*s2)));
        if (a != b) return a - b;
        ++s1; ++s2;
    }
    return static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2);
}

int APS5_VABI strncasecmp_nid_postfix(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && *s2) {
        unsigned char a = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(*s1)));
        unsigned char b = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(*s2)));
        if (a != b) return a - b;
        ++s1; ++s2; --n;
    }
    if (!n) return 0;
    return static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2);
}

char* APS5_VABI strdup_nid_postfix(const char* s) {
    std::size_t len = std::strlen(s) + 1;
    char* copy = static_cast<char*>(ApplicationHeapAllocate_nid_no_patch(len));
    std::memcpy(copy, s, len);
    return copy;
}

}
