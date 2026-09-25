#ifndef CORE_LIBS_PRX_LIBKERNEL_PTHREAD_PTHREAD_HPP
#define CORE_LIBS_PRX_LIBKERNEL_PTHREAD_PTHREAD_HPP

#include <sched.h>
#include "SceTypes.hpp"
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include "prx/libkernel/Time/include/ThreadClock.hpp"

enum class MutexType : std::uint32_t {
    ErrorCheck = 1,
    Recursive = 2,
    Normal = 3,
};

struct PthreadMutexattrPrivate {
    MutexType type;
};

struct PthreadMutexPrivate {
    std::recursive_timed_mutex _rmtx;
    std::timed_mutex _mtx;
    MutexType _type;
    std::atomic<std::thread::id> _owner;
    int _count;

    PthreadMutexPrivate() : _type(MutexType::Normal), _count(0) {}
};

struct PthreadCondattrPrivate {
    int _clockid;
};

struct PthreadCondPrivate {
    std::condition_variable_any _cv;
};

struct PthreadAttrPrivate {
    void* stackAddress = nullptr;
    std::size_t _stacksize = 0;
    std::size_t _guardsize = 0;
    int _detachstate = 0;
    int _schedpriority = 0;
    int _schedpolicy = 0;
    int _inheritsched = 0;
};

struct PthreadPrivate {
    int cancellationState = 0;
    int cancellationType = 0;
    int cpuClockId = 0;
    ~PthreadPrivate() { GuestThreadClocks::Release(cpuClockId); }
#ifdef _WIN32
    void* nativeHandle = nullptr;
    std::thread::id threadId;
    std::atomic<unsigned> references{2};
#else
    std::thread _thr;
#endif
    void* stackAddress = nullptr;
    std::size_t stackSize = 0;
    std::size_t guardSize = 0;
    std::atomic<bool> _finished;
    void* _retval;
    bool _detached;
    std::mutex _join_mtx;
    std::condition_variable _join_cv;

    PthreadPrivate() : _finished(false), _retval(nullptr), _detached(false) {}
};

#endif
