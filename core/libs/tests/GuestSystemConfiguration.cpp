#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <array>
#include <cstring>
#include <vector>
#include "prx/libc/include/General.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
struct GuestResourceLimit { std::uint64_t current; std::uint64_t maximum; };
extern "C" {
int APS5_VABI getrlimit_nid_postfix(int, GuestResourceLimit*);
std::int64_t APS5_VABI sysconf_nid_postfix(int);
int APS5_VABI getpagesize_nid_postfix();
int* APS5_VABI __error_nid_postfix();
int APS5_VABI sysctl_nid_postfix(const int*, unsigned, void*, std::size_t*, const void*, std::size_t);
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    struct GuardedLimit { GuestResourceLimit value; std::uint64_t guard; } limit{{0, 0}, UINT64_MAX};
    *__error_nid_postfix() = 13;
    Require(getrlimit_nid_postfix(8, &limit.value) == 0);
    Require(limit.value.current > 0 && limit.value.maximum >= limit.value.current && limit.guard == UINT64_MAX);
    Require(*__error_nid_postfix() == 13);
    Require(getrlimit_nid_postfix(3, &limit.value) == 0 && limit.value.current > 0);
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION stack{};
    Require(VirtualQuery(&limit, &stack, sizeof(stack)) == sizeof(stack));
    const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    const auto actualReserve = reinterpret_cast<std::uintptr_t>(tib->StackBase) -
        reinterpret_cast<std::uintptr_t>(stack.AllocationBase);
    Require(limit.value.current == actualReserve && limit.value.maximum == actualReserve);
#endif
    const auto saved = limit.value;
    Require(getrlimit_nid_postfix(-1, &limit.value) == -1 && *__error_nid_postfix() == 22);
    Require(getrlimit_nid_postfix(15, &limit.value) == -1 && *__error_nid_postfix() == 22);
    Require(getrlimit_nid_postfix(8, nullptr) == -1 && *__error_nid_postfix() == 14);
    Require(getrlimit_nid_postfix(9, &limit.value) == -1 && *__error_nid_postfix() == 45);
    Require(limit.value.current == saved.current && limit.value.maximum == saved.maximum && limit.guard == UINT64_MAX);
    *__error_nid_postfix() = 13;
    Require(sysconf_nid_postfix(47) == 0x4000);
    Require(getpagesize_nid_postfix() == sysconf_nid_postfix(47));
    Require(sysconf_nid_postfix(57) > 0);
    Require(sysconf_nid_postfix(58) > 0);
    Require(sysconf_nid_postfix(121) > 0);
    Require(*__error_nid_postfix() == 13);
    Require(sysconf_nid_postfix(-1) == -1); // verifies full-width signed return
    Require(*__error_nid_postfix() == 22);
    Require(sysconf_nid_postfix(0x7fffffff) == -1);
    for (const int identifier : {3, 7}) {
        const int name[] = {6, identifier};
        std::size_t size = 0;
        *__error_nid_postfix() = 13;
        Require(sysctl_nid_postfix(name, 2, nullptr, &size, nullptr, 0) == 0 && size == 4);
        std::int32_t value = 0;
        Require(sysctl_nid_postfix(name, 2, &value, &size, nullptr, 0) == 0);
        Require(value == sysconf_nid_postfix(identifier == 3 ? 58 : 47));
        Require(*__error_nid_postfix() == 13);
        std::array<unsigned char, 4> small{0xff, 0xff, 0xff, 0xff};
        size = 2;
        Require(sysctl_nid_postfix(name, 2, small.data(), &size, nullptr, 0) == -1);
        Require(*__error_nid_postfix() == 12 && size == 2);
        Require(std::memcmp(small.data(), &value, 2) == 0 && small[2] == 0xff && small[3] == 0xff);
        Require(sysctl_nid_postfix(name, 2, &value, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
        Require(sysctl_nid_postfix(name, 2, nullptr, nullptr, &value, sizeof(value)) == -1 && *__error_nid_postfix() == 1);
    }
    const int unknown[] = {6, 0x7fffffff};
    std::size_t unchanged = 77;
    Require(sysctl_nid_postfix(unknown, 2, nullptr, &unchanged, nullptr, 0) == -1);
    Require(*__error_nid_postfix() == 2 && unchanged == 77);
    Require(sysctl_nid_postfix(nullptr, 2, nullptr, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
    Require(sysctl_nid_postfix(unknown, 25, nullptr, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 22);
    const int executable[] = {1, 14, 12, -1};
    std::size_t size = 0;
    Require(sysctl_nid_postfix(executable, 4, nullptr, &size, nullptr, 0) == 0 && size > 1);
    std::vector<char> path(size);
    Require(sysctl_nid_postfix(executable, 4, path.data(), &size, nullptr, 0) == 0);
    Require(path.front() == '/' && path.back() == '\0' && std::strlen(path.data()) + 1 == size);
    Require(std::filesystem::is_regular_file(ResolvePath_nid_no_patch(path.data())));
}
