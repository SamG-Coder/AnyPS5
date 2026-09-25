#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <condition_variable>
#include <map>
#include <mutex>
#include <new>
#include <thread>
#include <climits>

struct PthreadRwlockPrivate {
    std::mutex mutex;
    std::condition_variable changed;
    std::map<std::thread::id, unsigned> readers;
    std::thread::id writer;
    unsigned waiting = 0;
    unsigned waitingWriters = 0;
};
namespace {
std::mutex initialization;
PthreadRwlock Resolve(PthreadRwlock* lock) {
    std::lock_guard guard(initialization);
    if (!*lock) *lock = new PthreadRwlockPrivate;
    return *lock;
}
int Lock(PthreadRwlock* lock, bool write, bool tryOnly) {
    if (!lock) return 22;
    try {
        auto& state = *Resolve(lock);
        std::unique_lock guard(state.mutex);
        const auto self = std::this_thread::get_id();
        const bool ownsRead = state.readers.contains(self);
        if (state.writer == self || (write && ownsRead)) return tryOnly ? 16 : 11;
        const auto ready = [&] {
            return state.writer == std::thread::id{} &&
                (write ? state.readers.empty() : ownsRead || state.waitingWriters == 0);
        };
        if (!ready()) {
            if (tryOnly) return 16;
            ++state.waiting;
            if (write) ++state.waitingWriters;
            state.changed.wait(guard, ready);
            --state.waiting;
            if (write) --state.waitingWriters;
        }
        if (write) state.writer = self;
        else {
            auto& count = state.readers[self];
            if (count == UINT_MAX) return 35;
            ++count;
        }
        return 0;
    } catch (const std::bad_alloc&) { return 12; }
}
}
extern "C" {
int APS5_VABI pthread_rwlock_init_nid_postfix(PthreadRwlock* lock, const PthreadRwlockattr* attr) {
    if (!lock || (attr && !*attr)) return 22;
    if (attr) return 45;
    try { *lock = new PthreadRwlockPrivate; return 0; }
    catch (const std::bad_alloc&) { return 12; }
}
int APS5_VABI pthread_rwlock_destroy_nid_postfix(PthreadRwlock* lock) {
    if (!lock) return 22;
    std::lock_guard initGuard(initialization);
    if (!*lock) return 0;
    auto* state = *lock;
    {
        std::lock_guard guard(state->mutex);
        if (state->writer != std::thread::id{} || !state->readers.empty() || state->waiting) return 16;
    }
    delete state;
    *lock = nullptr;
    return 0;
}
int APS5_VABI pthread_rwlock_rdlock_nid_postfix(PthreadRwlock* lock) { return Lock(lock, false, false); }
int APS5_VABI pthread_rwlock_wrlock_nid_postfix(PthreadRwlock* lock) { return Lock(lock, true, false); }
int APS5_VABI pthread_rwlock_tryrdlock_nid_postfix(PthreadRwlock* lock) { return Lock(lock, false, true); }
int APS5_VABI pthread_rwlock_trywrlock_nid_postfix(PthreadRwlock* lock) { return Lock(lock, true, true); }
int APS5_VABI pthread_rwlock_unlock_nid_postfix(PthreadRwlock* lock) {
    if (!lock) return 22;
    PthreadRwlock value;
    { std::lock_guard initGuard(initialization); value = *lock; }
    if (!value) return 1;
    auto& state = *value;
    std::lock_guard guard(state.mutex);
    const auto self = std::this_thread::get_id();
    if (state.writer == self) state.writer = {};
    else {
        const auto found = state.readers.find(self);
        if (found == state.readers.end()) return 1;
        if (--found->second == 0) state.readers.erase(found);
    }
    state.changed.notify_all();
    return 0;
}
}