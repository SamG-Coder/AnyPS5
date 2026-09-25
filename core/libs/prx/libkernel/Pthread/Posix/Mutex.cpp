#include <cstdint>
#include <cstddef>
#include <limits>
#include <mutex>
#include "SceTypes.hpp"

extern "C" {
int APS5_VABI scePthreadMutexattrInit(PthreadMutexattr*);
int APS5_VABI scePthreadMutexattrDestroy(PthreadMutexattr*);
int APS5_VABI scePthreadMutexattrSettype(PthreadMutexattr*, int);
int APS5_VABI scePthreadMutexattrSetprotocol(PthreadMutexattr*, int);
int APS5_VABI scePthreadMutexInit(PthreadMutex*, const PthreadMutexattr*, const char*);
int APS5_VABI scePthreadMutexDestroy(PthreadMutex*);
int APS5_VABI scePthreadMutexLock(PthreadMutex*);
int APS5_VABI scePthreadMutexTrylock(PthreadMutex*);
int APS5_VABI scePthreadMutexTimedlock(PthreadMutex*, KernelUseconds);
int APS5_VABI scePthreadMutexUnlock(PthreadMutex*);
int APS5_VABI clock_gettime_nid_postfix(int, KernelTimespec*);

static int MutexResult(int result) { return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff; }

static int EnsureMutex(PthreadMutex* mutex) {
    static auto* gate = new std::mutex;
    std::lock_guard lock(*gate);
    if (*mutex) return 0;
    return scePthreadMutexInit(mutex, nullptr, nullptr);
}

int APS5_VABI pthread_mutex_destroy_nid_postfix(PthreadMutex* mutex) {
    return MutexResult(scePthreadMutexDestroy(mutex));
}

int APS5_VABI pthread_mutex_init_nid_postfix(PthreadMutex* mutex, const PthreadMutexattr* attr) {
    return MutexResult(scePthreadMutexInit(mutex, attr, nullptr));
}

int APS5_VABI pthread_mutex_lock_nid_postfix(PthreadMutex* mutex) {
    if (!mutex) return 22;
    if (const int created = EnsureMutex(mutex)) return MutexResult(created);
    return MutexResult(scePthreadMutexLock(mutex));
}

int APS5_VABI pthread_mutex_trylock_nid_postfix(PthreadMutex* mutex) {
    if (!mutex) return 22;
    if (const int created = EnsureMutex(mutex)) return MutexResult(created);
    return MutexResult(scePthreadMutexTrylock(mutex));
}

int APS5_VABI pthread_mutex_timedlock_nid_postfix(PthreadMutex* mutex, const KernelTimespec* abstime) {
    if (!mutex || !abstime || abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000) return 22;
    if (const int created = EnsureMutex(mutex)) return MutexResult(created);
    KernelTimespec now{};
    if (clock_gettime_nid_postfix(0, &now) != 0) return 22;
    auto seconds = abstime->tv_sec - now.tv_sec;
    auto nanoseconds = abstime->tv_nsec - now.tv_nsec;
    if (nanoseconds < 0) { --seconds; nanoseconds += 1000000000; }
    if (seconds < 0) {
        const int result = MutexResult(scePthreadMutexTrylock(mutex));
        return result == 16 ? 60 : result;
    }
    const auto usec = static_cast<unsigned long long>(seconds) * 1000000ull
        + static_cast<unsigned long long>(nanoseconds) / 1000ull;
    const auto limited = usec > std::numeric_limits<unsigned>::max()
        ? std::numeric_limits<unsigned>::max() : static_cast<unsigned>(usec);
    return MutexResult(scePthreadMutexTimedlock(mutex, limited));
}

int APS5_VABI pthread_mutex_unlock_nid_postfix(PthreadMutex* mutex) {
    return MutexResult(scePthreadMutexUnlock(mutex));
}

int APS5_VABI pthread_mutexattr_destroy_nid_postfix(PthreadMutexattr* attr) {
    return MutexResult(scePthreadMutexattrDestroy(attr));
}

int APS5_VABI pthread_mutexattr_init_nid_postfix(PthreadMutexattr* attr) {
    return MutexResult(scePthreadMutexattrInit(attr));
}

int APS5_VABI pthread_mutexattr_setprotocol_nid_postfix(PthreadMutexattr* attr, int protocol) {
    return MutexResult(scePthreadMutexattrSetprotocol(attr, protocol));
}

int APS5_VABI pthread_mutexattr_settype_nid_postfix(PthreadMutexattr* attr, int type) {
    return MutexResult(scePthreadMutexattrSettype(attr, type));
}

}
