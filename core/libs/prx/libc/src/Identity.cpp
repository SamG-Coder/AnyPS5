#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <array>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace {
std::uint32_t WindowsIdentity(bool effective, bool group) {
    HANDLE handle = nullptr;
    if (effective && !OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &handle) &&
        GetLastError() != ERROR_NO_TOKEN)
        throw std::runtime_error("identity: cannot query thread token");
    if (!handle && !OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &handle))
        throw std::runtime_error("identity: cannot query process token");
    const std::unique_ptr<void, decltype(&CloseHandle)> token(handle, CloseHandle);
    const auto kind = group ? TokenPrimaryGroup : TokenUser;
    DWORD size = 0;
    GetTokenInformation(handle, kind, nullptr, 0, &size);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || size == 0)
        throw std::runtime_error("identity: cannot size token information");
    std::vector<unsigned char> information(size);
    if (!GetTokenInformation(handle, kind, information.data(), size, &size))
        throw std::runtime_error("identity: cannot read token information");
    PSID sid = group ? reinterpret_cast<TOKEN_PRIMARY_GROUP*>(information.data())->PrimaryGroup :
        reinterpret_cast<TOKEN_USER*>(information.data())->User.Sid;
    if (!IsValidSid(sid)) throw std::runtime_error("identity: invalid token SID");
    const auto* bytes = static_cast<const unsigned char*>(sid);
    const std::vector<unsigned char> key(bytes, bytes + GetLengthSid(sid));
    static std::mutex mutex;
    static std::array<std::map<std::vector<unsigned char>, std::uint32_t>, 2> identities;
    std::lock_guard lock(mutex);
    auto& table = identities[group ? 1 : 0];
    const auto found = table.find(key);
    if (found != table.end()) return found->second;
    if (table.size() >= UINT32_MAX - 1000u) throw std::runtime_error("identity: exhausted IDs");
    const auto value = static_cast<std::uint32_t>(1000u + table.size());
    table.emplace(key, value);
    return value;
}
}
#else
#include <unistd.h>
#endif

extern "C" {
std::uint32_t APS5_VABI getuid_nid_postfix() {
#ifdef _WIN32
    return WindowsIdentity(false, false);
#else
    return ::getuid();
#endif
}
std::uint32_t APS5_VABI geteuid_nid_postfix() {
#ifdef _WIN32
    return WindowsIdentity(true, false);
#else
    return ::geteuid();
#endif
}
std::uint32_t APS5_VABI getgid_nid_postfix() {
#ifdef _WIN32
    return WindowsIdentity(false, true);
#else
    return ::getgid();
#endif
}
std::uint32_t APS5_VABI getegid_nid_postfix() {
#ifdef _WIN32
    return WindowsIdentity(true, true);
#else
    return ::getegid();
#endif
}
}
