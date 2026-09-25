#include "prx/libc/include/general/VabiMacros.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <cerrno>
#include <sys/resource.h>
#endif

extern "C" int* APS5_VABI __error_nid_postfix();

namespace {
struct GuestResourceLimit {
    std::uint64_t current;
    std::uint64_t maximum;
};
static_assert(sizeof(GuestResourceLimit) == 16);
constexpr std::uint64_t guestInfinity = INT64_MAX;

int Fail(int error) {
    *__error_nid_postfix() = error;
    return -1;
}
}

extern "C" int APS5_VABI getrlimit_nid_postfix(int resource, GuestResourceLimit* output) {
    if (resource < 0 || resource >= 15) return Fail(22);
    if (!output) return Fail(14);
    GuestResourceLimit result{};
#ifdef _WIN32
    if (resource == 8) {
        result = {8192, 8192};
    } else if (resource == 3) {
        const auto* image = reinterpret_cast<const std::byte*>(GetModuleHandleW(nullptr));
        if (!image) return Fail(5);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0) return Fail(5);
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            return Fail(5);
        SYSTEM_INFO info{};
        GetSystemInfo(&info);
        const std::uint64_t granularity = info.dwAllocationGranularity;
        const std::uint64_t requested = nt->OptionalHeader.SizeOfStackReserve;
        if (!granularity || !requested || requested > guestInfinity - granularity + 1) return Fail(5);
        const auto reserved = ((requested + granularity - 1) / granularity) * granularity;
        result = {reserved, reserved};
    } else {
        return Fail(45);
    }
#else
    constexpr int nativeResources[] = {RLIMIT_CPU, RLIMIT_FSIZE, RLIMIT_DATA, RLIMIT_STACK,
        RLIMIT_CORE, RLIMIT_RSS, RLIMIT_MEMLOCK, RLIMIT_NPROC, RLIMIT_NOFILE, -1, RLIMIT_AS,
        -1, -1, -1, -1};
    const int nativeResource = nativeResources[resource];
    if (nativeResource < 0) return Fail(45);
    const int saved = *__error_nid_postfix();
    struct rlimit native{};
    if (::getrlimit(nativeResource, &native) != 0) return Fail(errno == EINVAL ? 22 : 5);
    const auto convert = [](rlim_t value) -> std::uint64_t {
        return value == RLIM_INFINITY ? guestInfinity : std::min<std::uint64_t>(value, guestInfinity);
    };
    result = {convert(native.rlim_cur), convert(native.rlim_max)};
    *__error_nid_postfix() = saved;
#endif
    *output = result;
    return 0;
}
