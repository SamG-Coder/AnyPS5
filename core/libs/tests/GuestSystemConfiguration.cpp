#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <array>
#include <cstring>
#include <vector>
#include <cstdio>
#include <climits>
#include <chrono>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif
#include "prx/libc/include/General.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
struct GuestResourceLimit { std::uint64_t current; std::uint64_t maximum; };
extern "C" {
int APS5_VABI usleep_nid_postfix(std::uint32_t);
std::uint32_t APS5_VABI getuid_nid_postfix();
std::uint32_t APS5_VABI geteuid_nid_postfix();
std::uint32_t APS5_VABI getgid_nid_postfix();
std::uint32_t APS5_VABI getegid_nid_postfix();
int APS5_VABI isatty_nid_postfix(int);
int APS5_VABI socket_nid_postfix(int, int, int);
int APS5_VABI close_nid_postfix(int);
int APS5_VABI getrlimit_nid_postfix(int, GuestResourceLimit*);
std::int64_t APS5_VABI sysconf_nid_postfix(int);
int APS5_VABI getpagesize_nid_postfix();
int* APS5_VABI __error_nid_postfix();
int APS5_VABI sysctl_nid_postfix(const int*, unsigned, void*, std::size_t*, const void*, std::size_t);
int APS5_VABI sysctlbyname_nid_postfix(const char*, void*, std::size_t*, const void*, std::size_t);
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    Require(usleep_nid_postfix(0) == 0);
    for (const std::uint32_t delay : {500u, 1500u, 1001000u}) {
        const auto start = std::chrono::steady_clock::now();
        Require(usleep_nid_postfix(delay) == 0);
        Require(std::chrono::steady_clock::now() - start >= std::chrono::microseconds(delay));
    }
    const auto user = getuid_nid_postfix();
    const auto group = getgid_nid_postfix();
#ifdef _WIN32
    Require(user >= 1000 && group >= 1000);
    Require(geteuid_nid_postfix() == user && getegid_nid_postfix() == group);
    Require(ImpersonateSelf(SecurityImpersonation));
    Require(geteuid_nid_postfix() == user && getegid_nid_postfix() == group);
    Require(getuid_nid_postfix() == user && getgid_nid_postfix() == group);
    Require(RevertToSelf());
#else
    Require(user == ::getuid() && group == ::getgid());
    Require(geteuid_nid_postfix() == ::geteuid() && getegid_nid_postfix() == ::getegid());
#endif
    Require(isatty_nid_postfix(-1) == 0 && *__error_nid_postfix() == 9);
    Require(isatty_nid_postfix(INT_MAX) == 0 && *__error_nid_postfix() == 9);
    auto* file = std::tmpfile();
    Require(file != nullptr);
#ifdef _WIN32
    const int descriptor = _fileno(file);
#else
    const int descriptor = fileno(file);
#endif
    Require(isatty_nid_postfix(descriptor) == 0 && *__error_nid_postfix() == 25);
    Require(std::fclose(file) == 0);
    Require(isatty_nid_postfix(descriptor) == 0 && *__error_nid_postfix() == 9);
#ifdef _WIN32
    const int nullDevice = _open("NUL", _O_RDWR);
    Require(nullDevice >= 0);
    Require(isatty_nid_postfix(nullDevice) == 0 && *__error_nid_postfix() == 25);
    Require(_close(nullDevice) == 0);
    int pipe[2]{};
    Require(_pipe(pipe, 256, _O_BINARY) == 0);
    for (const int end : pipe) {
        Require(isatty_nid_postfix(end) == 0 && *__error_nid_postfix() == 25);
        Require(_close(end) == 0);
    }
#endif
    const int socket = socket_nid_postfix(2, 2, 0);
    Require(socket >= 0);
    Require(isatty_nid_postfix(socket) == 0 && *__error_nid_postfix() == 25);
    Require(close_nid_postfix(socket) == 0);
    Require(isatty_nid_postfix(socket) == 0 && *__error_nid_postfix() == 9);
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
        const char* textualName = identifier == 3 ? "hw.ncpu" : "hw.pagesize";
        size = 0;
        Require(sysctlbyname_nid_postfix(textualName, nullptr, &size, nullptr, 0) == 0 && size == sizeof(value));
        std::array<std::int32_t, 2> named{0, -1};
        size = sizeof(named);
        Require(sysctlbyname_nid_postfix(textualName, named.data(), &size, nullptr, 0) == 0);
        Require(named[0] == value && named[1] == -1 && size == sizeof(value));
        Require(*__error_nid_postfix() == 13);
        size = 2;
        Require(sysctlbyname_nid_postfix(textualName, small.data(), &size, nullptr, 0) == -1);
        Require(*__error_nid_postfix() == 12 && size == 2);
        Require(std::memcmp(small.data(), &value, 2) == 0 && small[2] == 0xff && small[3] == 0xff);
        Require(sysctlbyname_nid_postfix(textualName, &value, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
        Require(sysctlbyname_nid_postfix(textualName, nullptr, nullptr, &value, sizeof(value)) == -1 && *__error_nid_postfix() == 1);
        Require(sysctlbyname_nid_postfix(textualName, nullptr, nullptr, nullptr, 1) == -1 && *__error_nid_postfix() == 22);
        small.fill(0xff);
        size = 2;
        Require(sysctl_nid_postfix(name, 2, small.data(), &size, nullptr, 0) == -1);
        Require(*__error_nid_postfix() == 12 && size == 2);
        Require(std::memcmp(small.data(), &value, 2) == 0 && small[2] == 0xff && small[3] == 0xff);
        Require(sysctl_nid_postfix(name, 2, &value, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
        Require(sysctl_nid_postfix(name, 2, nullptr, nullptr, &value, sizeof(value)) == -1 && *__error_nid_postfix() == 1);
    }
    const int unknown[] = {6, 0x7fffffff};
    std::size_t unchanged = 77;
    for (const auto* invalid : {"", "HW.NCPU", "hw.ncpu.extra", "hw."}) {
        Require(sysctlbyname_nid_postfix(invalid, nullptr, &unchanged, nullptr, 0) == -1);
        Require(*__error_nid_postfix() == 2 && unchanged == 77);
    }
    Require(sysctlbyname_nid_postfix(nullptr, nullptr, &unchanged, nullptr, 0) == -1);
    Require(*__error_nid_postfix() == 14 && unchanged == 77);
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
