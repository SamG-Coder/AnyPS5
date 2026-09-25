#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libc/include/VarArgsAbi.hpp"
#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#ifdef _WIN32
#include "prx/libc/include/WindowsFormatting.hpp"
#else
#include <syslog.h>
#endif

extern "C" char* APS5_VABI strerror_nid_postfix(int);
extern "C" const char* APS5_VABI getprogname_nid_postfix();
namespace {
std::atomic<unsigned> logMask{255};
std::mutex logMutex;
struct RestoreErrno { int value = errno; ~RestoreErrno() { errno = value; } };
std::string ExpandError(const char* format, int error) {
    std::string result;
    while (*format) {
        if (*format == '%' && format[1] == 'm') {
            for (const char* text = strerror_nid_postfix(error); *text; ++text) {
                result += *text;
                if (*text == '%') result += '%';
            }
            format += 2;
        } else if (*format == '%' && format[1] == '%') {
            result += "%%";
            format += 2;
        } else result += *format++;
    }
    return result;
}
}

extern "C" int APS5_VABI setlogmask_nid_postfix(int mask) {
    return static_cast<int>(mask ? logMask.exchange(static_cast<unsigned>(mask) & 255u) : logMask.load());
}
extern "C" void APS5_VABI vsyslog_nid_postfix(int priority, const char* format, VaList* args) {
    RestoreErrno restore;
    if (priority < 0 || (priority & ~0x3ff) || ((priority & 0x3f8) >> 3) >= 24)
        throw std::runtime_error("syslog: invalid priority or facility");
    if (!(logMask.load() & (1u << (priority & 7)))) return;
    if (!format || !args) throw std::runtime_error("syslog: null format or arguments");
    const auto expanded = ExpandError(format, restore.value);
#ifdef _WIN32
    std::string message;
    LibcDetail::FormatWindows(nullptr, 0, expanded.c_str(), args, &message);
    std::lock_guard lock(logMutex);
    const auto* name = getprogname_nid_postfix();
    if (std::fprintf(stderr, "%s: [syslog %d] ", name ? name : "", priority) < 0 ||
        std::fwrite(message.data(), 1, message.size(), stderr) != message.size() ||
        std::fputc('\n', stderr) == EOF || std::fflush(stderr) != 0)
        throw std::runtime_error("syslog: diagnostic output failed");
#else
    char* text = nullptr;
    if (::vasprintf(&text, expanded.c_str(), *reinterpret_cast<std::va_list*>(args)) < 0)
        throw std::runtime_error("syslog: formatting failed");
    const std::unique_ptr<char, decltype(&std::free)> message(text, std::free);
    ::syslog(priority, "%s", text);
#endif
}
extern "C" void APS5_VABI syslog_nid_postfix(int priority, const char* format, ...) {
#ifdef _WIN32
    __builtin_sysv_va_list args;
    __builtin_sysv_va_start(args, format);
#else
    std::va_list args;
    va_start(args, format);
#endif
    vsyslog_nid_postfix(priority, format, reinterpret_cast<VaList*>(args));
#ifdef _WIN32
    __builtin_sysv_va_end(args);
#else
    va_end(args);
#endif
}
