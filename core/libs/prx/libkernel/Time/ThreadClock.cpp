#include "include/ThreadClock.hpp"
#include "prx/libkernel/Pthread/include/Pthread.hpp"
#include <cerrno>
#include <climits>
#include <map>
#include <memory>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

extern "C" Pthread APS5_VABI scePthreadSelf();
namespace {
struct Clock {
#ifdef _WIN32
    HANDLE handle = nullptr;
    ~Clock() { if (handle) CloseHandle(handle); }
#else
    clockid_t native{};
#endif
};
std::mutex clockMutex;
std::map<int, std::unique_ptr<Clock>> clocks;
int nextId = INT_MIN;
}

void GuestThreadClocks::Release(int id) {
    if (!id) return;
    std::lock_guard lock(clockMutex);
    clocks.erase(id);
}

extern "C" int APS5_VABI pthread_getcpuclockid_nid_postfix(Pthread thread, int* output) {
    if (!thread) return 3;
    if (!output) return 22;
    if (thread->_finished.load()) return 3;
    std::lock_guard lock(clockMutex);
    if (thread->cpuClockId) { *output = thread->cpuClockId; return 0; }
    if (nextId == -1) return 11;
    try {
        auto clock = std::make_unique<Clock>();
#ifdef _WIN32
        const auto source = thread == scePthreadSelf() ? GetCurrentThread() : thread->nativeHandle;
        if (!source) return 3;
        if (!DuplicateHandle(GetCurrentProcess(), source, GetCurrentProcess(), &clock->handle,
            THREAD_QUERY_INFORMATION | SYNCHRONIZE, FALSE, 0)) return 3;
#else
        if (thread != scePthreadSelf() && !thread->_thr.joinable()) return 3;
        const int result = ::pthread_getcpuclockid(thread == scePthreadSelf() ? pthread_self() :
            thread->_thr.native_handle(), &clock->native);
        if (result) return result == ESRCH ? 3 : 22;
#endif
        const int id = nextId++;
        clocks.emplace(id, std::move(clock));
        thread->cpuClockId = id;
        *output = id;
        return 0;
    } catch (const std::bad_alloc&) { return 12; }
}

int GuestThreadClocks::Read(int id, KernelTimespec* output) {
    std::lock_guard lock(clockMutex);
    const auto found = clocks.find(id);
    if (found == clocks.end()) { errno = 22; return -1; }
#ifdef _WIN32
    const auto handle = found->second->handle;
    if (WaitForSingleObject(handle, 0) != WAIT_TIMEOUT) { errno = 22; return -1; }
    FILETIME creation{}, exit{}, kernel{}, user{};
    if (!GetThreadTimes(handle, &creation, &exit, &kernel, &user)) { errno = 22; return -1; }
    const auto ticks = ((std::uint64_t{kernel.dwHighDateTime} << 32) | kernel.dwLowDateTime) +
        ((std::uint64_t{user.dwHighDateTime} << 32) | user.dwLowDateTime);
    output->tv_sec = static_cast<std::int64_t>(ticks / 10000000);
    output->tv_nsec = static_cast<std::int64_t>((ticks % 10000000) * 100);
#else
    timespec value{};
    if (::clock_gettime(found->second->native, &value)) { errno = 22; return -1; }
    output->tv_sec = value.tv_sec;
    output->tv_nsec = value.tv_nsec;
#endif
    return 0;
}

int GuestThreadClocks::Resolution(int id, KernelTimespec* output) {
    KernelTimespec value{};
    if (Read(id, &value)) return -1;
#ifdef _WIN32
    DWORD adjustment = 0, increment = 0;
    BOOL disabled = FALSE;
    if (!GetSystemTimeAdjustment(&adjustment, &increment, &disabled)) { errno = 22; return -1; }
    value.tv_sec = increment / 10000000;
    value.tv_nsec = (increment % 10000000) * 100;
#else
    std::lock_guard lock(clockMutex);
    const auto found = clocks.find(id);
    if (found == clocks.end()) { errno = 22; return -1; }
    timespec native{};
    if (::clock_getres(found->second->native, &native)) { errno = 22; return -1; }
    value.tv_sec = native.tv_sec;
    value.tv_nsec = native.tv_nsec;
#endif
    if (output) *output = value;
    return 0;
}
