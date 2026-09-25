#include "prx/libc/include/general/VabiMacros.hpp"
#include <atomic>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <cerrno>
#endif

namespace {
const char* InitialName() {
#ifdef _WIN32
    static const std::string name = [] {
        std::wstring buffer(256, L'\0');
        for (;;) {
            const auto size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (!size) throw std::runtime_error("getprogname: cannot query executable path");
            if (size < buffer.size()) {
                buffer.resize(size);
                const auto bytes = std::filesystem::path(buffer).filename().u8string();
                return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
            }
            if (buffer.size() >= 32768) throw std::runtime_error("getprogname: executable path is too long");
            buffer.resize(buffer.size() * 2);
        }
    }();
    return name.c_str();
#else
    return program_invocation_short_name;
#endif
}
std::atomic<const char*>& ProgramName() {
    static std::atomic<const char*> name{InitialName()};
    return name;
}
}

extern "C" const char* APS5_VABI getprogname_nid_postfix() {
    return ProgramName().load();
}
extern "C" void APS5_VABI setprogname_nid_postfix(const char* name) {
    if (!name) throw std::runtime_error("setprogname: null name");
    const char* separator = std::strrchr(name, '/');
    ProgramName().store(separator ? separator + 1 : name);
}
