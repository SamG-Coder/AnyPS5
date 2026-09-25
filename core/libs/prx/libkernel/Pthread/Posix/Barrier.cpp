#include "prx/libc/include/general/VabiMacros.hpp"
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <new>

struct GuestBarrier {
    std::mutex mutex;
    std::condition_variable changed;
    unsigned count;
    unsigned arrived = 0;
    unsigned active = 0;
    std::uint64_t generation = 0;
    explicit GuestBarrier(unsigned value) : count(value) {}
};
struct GuestBarrierAttr { int shared = 0; };

extern "C" {
int APS5_VABI pthread_barrierattr_init_nid_postfix(GuestBarrierAttr** attr) {
    if (!attr) return 22;
    try { *attr = new GuestBarrierAttr; return 0; }
    catch (const std::bad_alloc&) { return 12; }
}
int APS5_VABI pthread_barrierattr_destroy_nid_postfix(GuestBarrierAttr** attr) {
    if (!attr || !*attr) return 22;
    delete *attr;
    *attr = nullptr;
    return 0;
}
int APS5_VABI pthread_barrierattr_getpshared_nid_postfix(GuestBarrierAttr* const* attr, int* value) {
    if (!attr || !*attr || !value) return 22;
    *value = (*attr)->shared;
    return 0;
}
int APS5_VABI pthread_barrierattr_setpshared_nid_postfix(GuestBarrierAttr** attr, int value) {
    if (!attr || !*attr || (value != 0 && value != 1)) return 22;
    if (value == 1) return 45;
    (*attr)->shared = value;
    return 0;
}
int APS5_VABI pthread_barrier_init_nid_postfix(GuestBarrier** barrier, GuestBarrierAttr* const* attr, unsigned count) {
    if (!barrier || count == 0 || (attr && !*attr)) return 22;
    if (attr && (*attr)->shared != 0) return 45;
    try { *barrier = new GuestBarrier(count); return 0; }
    catch (const std::bad_alloc&) { return 12; }
}
int APS5_VABI pthread_barrier_destroy_nid_postfix(GuestBarrier** barrier) {
    if (!barrier || !*barrier) return 22;
    auto* value = *barrier;
    {
        std::lock_guard lock(value->mutex);
        if (value->active) return 16;
    }
    delete value;
    *barrier = nullptr;
    return 0;
}
int APS5_VABI pthread_barrier_wait_nid_postfix(GuestBarrier** barrier) {
    if (!barrier || !*barrier) return 22;
    auto& value = **barrier;
    std::unique_lock lock(value.mutex);
    const auto generation = value.generation;
    ++value.active;
    if (++value.arrived == value.count) {
        value.arrived = 0;
        ++value.generation;
        --value.active;
        value.changed.notify_all();
        return -1;
    }
    value.changed.wait(lock, [&] { return value.generation != generation; });
    --value.active;
    return 0;
}
}
