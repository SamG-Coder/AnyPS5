#include <cstdint>
#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <new>
#include <cerrno>
#include <chrono>
#include <limits>
#include "SceTypes.hpp"

namespace {
constexpr std::uint32_t SemMagic = 0x09fa4012u;
constexpr unsigned SemValueMax = static_cast<unsigned>(std::numeric_limits<int>::max());

struct State {
    std::mutex mutex;
    std::condition_variable condition;
    unsigned value = 0;
    int waiters = 0;
};

struct GuestSem {
    std::uint32_t magic;
    std::uint32_t unused;
    State* state;
};
static_assert(sizeof(GuestSem) == 16);

GuestSem* Prepared(void* sem) {
    auto* guest = static_cast<GuestSem*>(sem);
    if (!guest || guest->magic != SemMagic || !guest->state) { errno = 22; return nullptr; }
    return guest;
}

bool Wait(State& state, std::unique_lock<std::mutex>& lock, const std::chrono::steady_clock::time_point* deadline) {
    ++state.waiters;
    bool acquired = false;
    if (!deadline) {
        state.condition.wait(lock, [&] { return state.value > 0; });
        acquired = true;
    } else {
        acquired = state.condition.wait_until(lock, *deadline, [&] { return state.value > 0; });
    }
    --state.waiters;
    if (!acquired) return false;
    --state.value;
    return true;
}
}

extern "C" {

int APS5_VABI sem_init_nid_postfix(void* sem, int pshared, unsigned int value) {
    if (!sem || (pshared != 0 && pshared != 1) || value > SemValueMax) { errno = 22; return -1; }
    auto* guest = static_cast<GuestSem*>(sem);
    if (guest->magic == SemMagic) { errno = 22; return -1; }
    auto* state = new (std::nothrow) State;
    if (!state) { errno = 12; return -1; }
    state->value = value;
    guest->unused = 0;
    guest->state = state;
    guest->magic = SemMagic;
    return 0;
}

int APS5_VABI sem_destroy_nid_postfix(void* sem) {
    auto* guest = Prepared(sem);
    if (!guest) return -1;
    State* state = nullptr;
    {
        std::lock_guard lock(guest->state->mutex);
        if (guest->state->waiters != 0) { errno = 16; return -1; }
        state = guest->state;
        guest->state = nullptr;
        guest->magic = 0;
    }
    delete state;
    return 0;
}

int APS5_VABI sem_wait_nid_postfix(void* sem) {
    auto* guest = Prepared(sem);
    if (!guest) return -1;
    std::unique_lock lock(guest->state->mutex);
    Wait(*guest->state, lock, nullptr);
    return 0;
}

int APS5_VABI sem_trywait_nid_postfix(void* sem) {
    auto* guest = Prepared(sem);
    if (!guest) return -1;
    std::lock_guard lock(guest->state->mutex);
    if (guest->state->value == 0) { errno = 35; return -1; }
    --guest->state->value;
    return 0;
}

int APS5_VABI sem_post_nid_postfix(void* sem) {
    auto* guest = Prepared(sem);
    if (!guest) return -1;
    std::lock_guard lock(guest->state->mutex);
    if (guest->state->value == SemValueMax) { errno = 84; return -1; }
    ++guest->state->value;
    guest->state->condition.notify_one();
    return 0;
}

int APS5_VABI sem_getvalue_nid_postfix(void* sem, int* value) {
    auto* guest = Prepared(sem);
    if (!guest || !value) { errno = 22; return -1; }
    std::lock_guard lock(guest->state->mutex);
    *value = static_cast<int>(guest->state->value);
    return 0;
}

int APS5_VABI sem_timedwait_nid_postfix(void* sem, const KernelTimespec* abstime) {
    auto* guest = Prepared(sem);
    if (!guest || !abstime || abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000) { errno = 22; return -1; }
    const auto now = std::chrono::system_clock::now();
    const auto deadline = std::chrono::system_clock::time_point(std::chrono::seconds(abstime->tv_sec)
        + std::chrono::nanoseconds(abstime->tv_nsec));
    const auto remaining = deadline > now
        ? std::chrono::duration_cast<std::chrono::steady_clock::duration>(deadline - now)
        : std::chrono::steady_clock::duration::zero();
    const auto steadyDeadline = std::chrono::steady_clock::now() + remaining;
    std::unique_lock lock(guest->state->mutex);
    if (!Wait(*guest->state, lock, &steadyDeadline)) { errno = 60; return -1; }
    return 0;
}

int APS5_VABI sem_reltimedwait_np_nid_postfix(void* sem, std::uint32_t usec) {
    auto* guest = Prepared(sem);
    if (!guest) return -1;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::microseconds(usec);
    std::unique_lock lock(guest->state->mutex);
    if (!Wait(*guest->state, lock, &deadline)) { errno = 60; return -1; }
    return 0;
}

}
