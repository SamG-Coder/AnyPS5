#include "prx/libc/include/General.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

extern "C" int* APS5_VABI __error_nid_postfix();
extern "C" std::int64_t APS5_VABI sysconf_nid_postfix(int);

namespace {
int Fail(int error) {
    *__error_nid_postfix() = error;
    return -1;
}

int CopyValue(void* output, std::size_t* length, const void* value, std::size_t size) {
    if (!length) return output ? Fail(14) : 0;
    if (!output) { *length = size; return 0; }
    const auto copied = std::min(*length, size);
    if (copied) std::memcpy(output, value, copied);
    *length = copied;
    return copied == size ? 0 : Fail(12);
}

std::filesystem::path ExecutablePath() {
#ifdef _WIN32
    std::vector<wchar_t> buffer(256);
    for (;;) {
        const auto length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (!length) throw std::runtime_error("sysctl: cannot determine executable path");
        if (length < buffer.size()) return std::filesystem::path(buffer.data(), buffer.data() + length);
        if (buffer.size() >= 32768) throw std::runtime_error("sysctl: executable path exceeds platform limit");
        buffer.resize(buffer.size() * 2);
    }
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}
}

extern "C" int APS5_VABI sysctl_nid_postfix(const int* name, unsigned count,
    void* output, std::size_t* length, const void* replacement, std::size_t replacementSize) {
    if (count < 2 || count > 24) return Fail(22);
    if (!name || (output && !length)) return Fail(14);
    if (replacement) return Fail(1);
    if (replacementSize) return Fail(22);
    if (count == 2 && name[0] == 6 && (name[1] == 3 || name[1] == 7)) {
        const auto result = sysconf_nid_postfix(name[1] == 3 ? 58 : 47);
        if (result < 0) return -1;
        const auto value = static_cast<std::int32_t>(result);
        return CopyValue(output, length, &value, sizeof(value));
    }
    if (count == 4 && name[0] == 1 && name[1] == 14 && name[2] == 12) {
#ifdef _WIN32
        const auto pid = static_cast<int>(GetCurrentProcessId());
#else
        const auto pid = static_cast<int>(::getpid());
#endif
        if (name[3] != -1 && name[3] != pid) return Fail(3);
        const auto executable = std::filesystem::canonical(ExecutablePath());
        const auto root = std::filesystem::canonical(ResolvePath_nid_no_patch("/"));
        const auto relative = executable.lexically_relative(root);
        if (relative.empty() || *relative.begin() == "..") return Fail(45);
        const auto path = "/" + relative.generic_string();
        return CopyValue(output, length, path.c_str(), path.size() + 1);
    }
    return Fail(2);
}

extern "C" int APS5_VABI sysctlbyname_nid_postfix(const char* name,
    void* output, std::size_t* length, const void* replacement, std::size_t replacementSize) {
    if (!name) return Fail(14);
    int identifier;
    if (std::strcmp(name, "hw.ncpu") == 0) identifier = 3;
    else if (std::strcmp(name, "hw.pagesize") == 0) identifier = 7;
    else return Fail(2);
    const int mib[] = {6, identifier};
    return sysctl_nid_postfix(mib, 2, output, length, replacement, replacementSize);
}
