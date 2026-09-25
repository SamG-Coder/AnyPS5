#include "prx/libc/include/general/VabiMacros.hpp"
#include <atomic>
#include <csignal>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <stdexcept>
#include "../include/SignalAction.hpp"
#ifndef _WIN32
#include <pthread.h>
#endif

extern "C" int* APS5_VABI __error_nid_postfix();
extern "C" int APS5_VABI pthread_sigmask_nid_postfix(int, const std::uint32_t*, std::uint32_t*);
namespace {
using GuestHandler = GuestSignals::Handler;
using Action = GuestSignals::Action;
const Action defaultAction{};
std::atomic<const Action*> actions[32]{};
static_assert(std::atomic<const Action*>::is_always_lock_free);
std::vector<std::unique_ptr<const Action>> retainedActions;
std::mutex registration;
const Action* CurrentAction(int guest) {
    const auto* action = actions[guest].load();
    return action ? action : &defaultAction;
}
#ifdef _WIN32
thread_local GuestSignals::Mask blocked{};
thread_local std::atomic<std::uint32_t> blockedDelivery{0};
thread_local std::atomic<std::uint32_t> pending{0};
static_assert(std::atomic<std::uint32_t>::is_always_lock_free);
#endif
int NativeSignal(int guest) {
    switch (guest) {
        case 2: return SIGINT;
        case 4: return SIGILL;
        case 6: return SIGABRT;
        case 8: return SIGFPE;
        case 11: return SIGSEGV;
        case 15: return SIGTERM;
        default: return 0;
    }
}
void Dispatch(int native) {
    int guest = 0;
    for (int candidate : {2, 4, 6, 8, 11, 15})
        if (NativeSignal(candidate) == native) { guest = candidate; break; }
    if (!guest) return;
#ifdef _WIN32
    if (blockedDelivery.load() & (std::uint32_t{1} << (guest - 1))) {
        if (guest == 4 || guest == 8 || guest == 11) std::terminate();
        std::signal(native, Dispatch);
        pending.fetch_or(std::uint32_t{1} << (guest - 1));
        return;
    }
#endif
    const auto* action = CurrentAction(guest);
    if (action->flags & 4) {
        const auto* expected = action;
        actions[guest].compare_exchange_strong(expected, &defaultAction);
    }
#ifdef _WIN32
    std::signal(native, (action->flags & 4) ? SIG_DFL : Dispatch);
    const auto saved = blocked;
    auto effective = saved;
    for (unsigned index = 0; index < 4; ++index) effective[index] |= action->mask[index];
    if (!(action->flags & 16)) effective[0] |= std::uint32_t{1} << (guest - 1);
    effective[0] &= ~((std::uint32_t{1} << 8) | (std::uint32_t{1} << 16));
    blocked = effective;
    blockedDelivery.store(effective[0]);
#endif
    if (reinterpret_cast<std::uintptr_t>(action->handler) > 1) action->handler(guest);
#ifdef _WIN32
    pthread_sigmask_nid_postfix(3, saved.data(), nullptr);
#endif
}
}
GuestSignals::Mask GuestSignals::CaptureMask() {
#ifdef _WIN32
    return blocked;
#else
    return {};
#endif
}
void GuestSignals::InheritMask(const Mask& mask) {
#ifdef _WIN32
    blocked = mask;
    blockedDelivery.store(mask[0]);
    pending.store(0);
#else
    (void)mask;
#endif
}
extern "C" {
int APS5_VABI pthread_sigmask_nid_postfix(int how, const std::uint32_t* set, std::uint32_t* previous) {
    if (set && (how < 1 || how > 3)) return 22;
#ifdef _WIN32
    const auto original = blocked;
    if (set) {
        auto next = original;
        for (unsigned index = 0; index < 4; ++index) {
            if (how == 1) next[index] |= set[index];
            else if (how == 2) next[index] &= ~set[index];
            else next[index] = set[index];
        }
        next[0] &= ~((std::uint32_t{1} << 8) | (std::uint32_t{1} << 16));
        blocked = next;
        blockedDelivery.store(next[0]);
    }
    if (previous) for (unsigned index = 0; index < 4; ++index) previous[index] = original[index];
    for (const int guest : {2, 4, 6, 8, 11, 15}) {
        const auto bit = std::uint32_t{1} << (guest - 1);
        if (!(blocked[0] & bit) && (pending.fetch_and(~bit) & bit)) std::raise(NativeSignal(guest));
    }
    return 0;
#else
    sigset_t native{}, old{};
    sigemptyset(&native);
    if (set) {
        for (int guest = 1; guest <= 128; ++guest) {
            if (!(set[(guest - 1) / 32] & (std::uint32_t{1} << ((guest - 1) % 32)))) continue;
            if (guest == 9 || guest == 17) continue;
            const int signal = NativeSignal(guest);
            if (!signal) throw std::runtime_error("pthread_sigmask: unsupported Linux signal mapping");
            sigaddset(&native, signal);
        }
    }
    const int result = ::pthread_sigmask(how == 1 ? SIG_BLOCK : how == 2 ? SIG_UNBLOCK : SIG_SETMASK,
        set ? &native : nullptr, &old);
    if (result) return 22;
    if (previous) {
        for (unsigned index = 0; index < 4; ++index) previous[index] = 0;
        for (const int guest : {2, 4, 6, 8, 11, 15})
            if (sigismember(&old, NativeSignal(guest))) previous[0] |= std::uint32_t{1} << (guest - 1);
    }
    return 0;
#endif
}
int APS5_VABI sigprocmask_nid_postfix(int how, const std::uint32_t* set, std::uint32_t* previous) {
    const int result = pthread_sigmask_nid_postfix(how, set, previous);
    if (result) { *__error_nid_postfix() = result; return -1; }
    return 0;
}
int APS5_VABI sigemptyset_nid_postfix(std::uint32_t* set) {
    if (!set) { *__error_nid_postfix() = 14; return -1; }
    for (unsigned index = 0; index < 4; ++index) set[index] = 0;
    return 0;
}
int APS5_VABI sigfillset_nid_postfix(std::uint32_t* set) {
    if (!set) { *__error_nid_postfix() = 14; return -1; }
    for (unsigned index = 0; index < 4; ++index) set[index] = UINT32_MAX;
    return 0;
}
int APS5_VABI sigaddset_nid_postfix(std::uint32_t* set, int signal) {
    if (!set) { *__error_nid_postfix() = 14; return -1; }
    if (signal < 1 || signal > 128) { *__error_nid_postfix() = 22; return -1; }
    const auto index = static_cast<unsigned>(signal - 1);
    set[index / 32] |= std::uint32_t{1} << (index % 32);
    return 0;
}
int APS5_VABI sigdelset_nid_postfix(std::uint32_t* set, int signal) {
    if (!set) { *__error_nid_postfix() = 14; return -1; }
    if (signal < 1 || signal > 128) { *__error_nid_postfix() = 22; return -1; }
    const auto index = static_cast<unsigned>(signal - 1);
    set[index / 32] &= ~(std::uint32_t{1} << (index % 32));
    return 0;
}
int APS5_VABI sigismember_nid_postfix(const std::uint32_t* set, int signal) {
    if (!set) { *__error_nid_postfix() = 14; return -1; }
    if (signal < 1 || signal > 128) { *__error_nid_postfix() = 22; return -1; }
    const auto index = static_cast<unsigned>(signal - 1);
    return (set[index / 32] >> (index % 32)) & 1u;
}
int APS5_VABI sigaction_nid_postfix(int guest, const Action* requested, Action* previous) {
    const int native = NativeSignal(guest);
    if (!native || (requested && reinterpret_cast<std::uintptr_t>(requested->handler) == UINTPTR_MAX)) {
        *__error_nid_postfix() = 22;
        return -1;
    }
    if (!requested) { if (previous) *previous = *CurrentAction(guest); return 0; }
    const Action next = *requested;
    if (next.flags & ~(2 | 4 | 16)) throw std::runtime_error("sigaction: unsupported action flags");
#ifdef _WIN32
    if (next.flags & 2) throw std::runtime_error("sigaction: Windows syscall restart is unsupported");
#endif
    std::lock_guard lock(registration);
    const Action* retained = nullptr;
    for (const auto& candidate : retainedActions) {
        if (candidate->handler == next.handler && candidate->flags == next.flags && candidate->mask == next.mask) {
            retained = candidate.get();
            break;
        }
    }
    if (!retained) {
        retainedActions.push_back(std::make_unique<const Action>(next));
        retained = retainedActions.back().get();
    }
    const auto* original = CurrentAction(guest);
    const auto address = reinterpret_cast<std::uintptr_t>(next.handler);
    auto hostHandler = address == 0 ? SIG_DFL : address == 1 ? SIG_IGN : Dispatch;
#ifdef _WIN32
    actions[guest].store(retained);
    const bool failed = std::signal(native, hostHandler) == SIG_ERR;
#else
    struct sigaction host{};
    host.sa_handler = hostHandler;
    sigemptyset(&host.sa_mask);
    for (int signal = 1; signal <= 128; ++signal) {
        if (!(next.mask[(signal - 1) / 32] & (std::uint32_t{1} << ((signal - 1) % 32)))) continue;
        if (signal == 9 || signal == 17) continue;
        const int mapped = NativeSignal(signal);
        if (!mapped) {
            throw std::runtime_error("sigaction: unsupported Linux mask mapping");
        }
        sigaddset(&host.sa_mask, mapped);
    }
    if (next.flags & 2) host.sa_flags |= SA_RESTART;
    if (next.flags & 4) host.sa_flags |= SA_RESETHAND;
    if (next.flags & 16) host.sa_flags |= SA_NODEFER;
    actions[guest].store(retained);
    const bool failed = ::sigaction(native, &host, nullptr) != 0;
#endif
    if (failed) {
        actions[guest].store(original);
        *__error_nid_postfix() = 22;
        return -1;
    }
    if (previous) *previous = *original;
#ifdef _WIN32
    if (address == 1) pending.fetch_and(~(std::uint32_t{1} << (guest - 1)));
#endif
    return 0;
}
GuestHandler APS5_VABI signal_nid_postfix(int guest, GuestHandler handler) {
#ifdef _WIN32
    const Action next{handler, 0, {}};
#else
    const Action next{handler, 2, {}};
#endif
    Action previous{};
    if (sigaction_nid_postfix(guest, &next, &previous) != 0)
        return reinterpret_cast<GuestHandler>(UINTPTR_MAX);
    return previous.handler;
}
int APS5_VABI raise_nid_postfix(int guest) {
    const int native = NativeSignal(guest);
    if (!native) { *__error_nid_postfix() = 22; return -1; }
#ifdef _WIN32
    const auto bit = std::uint32_t{1} << (guest - 1);
    if (blocked[0] & bit) {
        if (reinterpret_cast<std::uintptr_t>(CurrentAction(guest)->handler) != 1) pending.fetch_or(bit);
        return 0;
    }
#endif
    const int result = std::raise(native);
    if (result) *__error_nid_postfix() = 22;
    return result ? -1 : 0;
}
}
