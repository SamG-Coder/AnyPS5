#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include "prx/libc/include/ApplicationHeap.hpp"

extern "C" {
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

static std::array<char, 64> allocation;
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
int main() {
    std::array<void*, 10> api{};
    api.fill(reinterpret_cast<void*>(UnexpectedHeapCall));
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
