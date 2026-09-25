#include "prx/libc/include/general/VabiMacros.hpp"
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <climits>
#include <stdexcept>
#include "SceTypes.hpp"
#include "prx/libkernel/System/include/SignalAction.hpp"
using Handler = void (APS5_VABI *)(int);
extern "C" {
int APS5_VABI pthread_sigmask_nid_postfix(int, const std::uint32_t*, std::uint32_t*);
int APS5_VABI sigprocmask_nid_postfix(int, const std::uint32_t*, std::uint32_t*);
int APS5_VABI pthread_create_nid_postfix(Pthread*, const PthreadAttr*, PthreadEntry, void*);
int APS5_VABI pthread_join_nid_postfix(Pthread, void**);
int APS5_VABI sigemptyset_nid_postfix(std::uint32_t*);
int APS5_VABI sigfillset_nid_postfix(std::uint32_t*);
int APS5_VABI sigaddset_nid_postfix(std::uint32_t*, int);
int APS5_VABI sigdelset_nid_postfix(std::uint32_t*, int);
int APS5_VABI sigismember_nid_postfix(const std::uint32_t*, int);
Handler APS5_VABI signal_nid_postfix(int, Handler);
int APS5_VABI raise_nid_postfix(int);
int* APS5_VABI __error_nid_postfix();
int APS5_VABI sigaction_nid_postfix(int, const GuestSignals::Action*, GuestSignals::Action*);
}
volatile std::sig_atomic_t received = 0;
volatile std::sig_atomic_t deliveries = 0;
void APS5_VABI Callback(int value) { received = value; deliveries = deliveries + 1; }
static void Require(bool value) { if (!value) std::abort(); }
static volatile std::sig_atomic_t depth = 0;
static volatile std::sig_atomic_t maximumDepth = 0;
static volatile std::sig_atomic_t recursiveCalls = 0;
static std::array<std::uint32_t, 4> observedMask{};
static void APS5_VABI RecursiveCallback(int signal) {
    depth = depth + 1;
    if (depth > maximumDepth) maximumDepth = depth;
    recursiveCalls = recursiveCalls + 1;
    Require(pthread_sigmask_nid_postfix(0, nullptr, observedMask.data()) == 0);
    if (recursiveCalls == 1) Require(raise_nid_postfix(signal) == 0);
    depth = depth - 1;
}
static void APS5_VABI MaskedCallback(int) {
    Require(raise_nid_postfix(2) == 0);
    Require(received == 0);
}
static void CheckActions() {
    using GuestSignals::Action;
    struct GuardedAction { std::uint64_t before; Action action; std::uint64_t after; } old{123, {}, 456};
    Require(sigaction_nid_postfix(15, nullptr, &old.action) == 0);
    Require(old.action.handler == nullptr && old.action.flags == 0);
    Require(old.before == 123 && old.after == 456);
    for (const int flags : {0, 16}) {
        recursiveCalls = 0;
        maximumDepth = 0;
        Action next{RecursiveCallback, flags, {1u << 1, 0, 0, 0}};
        *__error_nid_postfix() = 13;
        Require(sigaction_nid_postfix(15, &next, &old.action) == 0);
        Require(*__error_nid_postfix() == 13);
        Action query{};
        Require(sigaction_nid_postfix(15, nullptr, &query) == 0);
        Require(query.handler == next.handler && query.flags == flags && query.mask == next.mask);
        Require(raise_nid_postfix(15) == 0);
        Require(recursiveCalls == 2 && depth == 0 && maximumDepth == (flags ? 2 : 1));
        Require(observedMask[0] == ((1u << 1) | (flags ? 0 : 1u << 14)));
        Require(pthread_sigmask_nid_postfix(0, nullptr, query.mask.data()) == 0);
        Require(query.mask == std::array<std::uint32_t, 4>{});
    }
    Action once{Callback, 4, {}};
    Require(sigaction_nid_postfix(15, &once, &once) == 0);
    Require(once.handler == RecursiveCallback && once.flags == 16);
    Require(raise_nid_postfix(15) == 0 && received == 15);
    Require(sigaction_nid_postfix(15, nullptr, &old.action) == 0);
    Require(old.action.handler == nullptr && old.action.flags == 0);
    const auto saved = old.action;
    Require(sigaction_nid_postfix(9, &saved, &old.action) == -1 && *__error_nid_postfix() == 22);
    Require(old.action.handler == saved.handler && old.before == 123 && old.after == 456);
    received = 0;
    deliveries = 0;
    Action interrupt{Callback, 0, {}};
    Action term{MaskedCallback, 0, {1u << 1, 0, 0, 0}};
    Require(sigaction_nid_postfix(2, &interrupt, nullptr) == 0);
    Require(sigaction_nid_postfix(15, &term, nullptr) == 0);
    Require(raise_nid_postfix(15) == 0 && received == 2 && deliveries == 1);
    Action unsupported{Callback, 64, {}};
    bool rejected = false;
    try { sigaction_nid_postfix(15, &unsupported, &old.action); }
    catch (const std::runtime_error&) { rejected = true; }
    Require(rejected);
    Require(sigaction_nid_postfix(15, nullptr, &old.action) == 0);
    Require(old.action.handler == MaskedCallback && old.action.mask == term.mask);
    Require(sigaction_nid_postfix(2, &saved, nullptr) == 0);
    Require(sigaction_nid_postfix(15, &saved, nullptr) == 0);
    received = 0;
    deliveries = 0;
}
static void* APS5_VABI CheckInheritedMask(void*) {
    std::array<std::uint32_t, 4> mask{};
    Require(pthread_sigmask_nid_postfix(0, nullptr, mask.data()) == 0);
    Require(mask[0] == (1u << 14));
    Require(pthread_sigmask_nid_postfix(2, mask.data(), nullptr) == 0);
    return nullptr;
}
int main() {
    CheckActions();
    struct Guarded { std::uint32_t before; std::array<std::uint32_t, 4> bits; std::uint32_t after; } set{
        0x12345678, {}, 0xabcdef01};
    auto* bits = set.bits.data();
    Require(sigfillset_nid_postfix(bits) == 0);
    for (const auto word : set.bits) Require(word == UINT32_MAX);
    for (int signal = 1; signal <= 128; ++signal) {
        Require(sigismember_nid_postfix(bits, signal) == 1);
        Require(sigdelset_nid_postfix(bits, signal) == 0);
        Require(sigismember_nid_postfix(bits, signal) == 0);
    }
    for (const auto word : set.bits) Require(word == 0);
    for (int signal = 1; signal <= 128; ++signal) {
        Require(sigemptyset_nid_postfix(bits) == 0);
        Require(sigaddset_nid_postfix(bits, signal) == 0);
        Require(sigaddset_nid_postfix(bits, signal) == 0);
        for (int candidate = 1; candidate <= 128; ++candidate)
            Require(sigismember_nid_postfix(bits, candidate) == (candidate == signal));
    }
    const auto saved = set.bits;
    for (const int signal : {INT_MIN, -1, 0, 129, INT_MAX}) {
        Require(sigaddset_nid_postfix(bits, signal) == -1 && *__error_nid_postfix() == 22);
        Require(sigdelset_nid_postfix(bits, signal) == -1 && *__error_nid_postfix() == 22);
        Require(sigismember_nid_postfix(bits, signal) == -1 && *__error_nid_postfix() == 22);
        Require(set.bits == saved);
    }
    Require(sigemptyset_nid_postfix(nullptr) == -1 && *__error_nid_postfix() == 14);
    Require(sigfillset_nid_postfix(nullptr) == -1 && *__error_nid_postfix() == 14);
    Require(sigaddset_nid_postfix(nullptr, 1) == -1 && *__error_nid_postfix() == 14);
    Require(sigdelset_nid_postfix(nullptr, 1) == -1 && *__error_nid_postfix() == 14);
    Require(sigismember_nid_postfix(nullptr, 1) == -1 && *__error_nid_postfix() == 14);
    Require(set.before == 0x12345678 && set.after == 0xabcdef01);
    const auto invalid = reinterpret_cast<Handler>(static_cast<std::uintptr_t>(-1));
    const auto ignore = reinterpret_cast<Handler>(std::uintptr_t{1});
    Require(signal_nid_postfix(9, Callback) == invalid);
    Require(*__error_nid_postfix() == 22);
    Require(signal_nid_postfix(15, Callback) != invalid);
    std::array<std::uint32_t, 4> mask{1u << 14, 0, 0, 0}, old{};
    *__error_nid_postfix() = 13;
    Require(pthread_sigmask_nid_postfix(99, mask.data(), old.data()) == 22);
    Require(*__error_nid_postfix() == 13);
    Require(sigprocmask_nid_postfix(99, mask.data(), old.data()) == -1 && *__error_nid_postfix() == 22);
    Require(pthread_sigmask_nid_postfix(1, mask.data(), old.data()) == 0);
    Require(old == std::array<std::uint32_t, 4>{});
    Require(raise_nid_postfix(15) == 0 && received == 0);
    Require(raise_nid_postfix(15) == 0 && received == 0);
    Pthread child = nullptr;
    Require(pthread_create_nid_postfix(&child, nullptr, CheckInheritedMask, nullptr) == 0);
    Require(pthread_join_nid_postfix(child, nullptr) == 0);
    Require(received == 0);
    Require(pthread_sigmask_nid_postfix(0, nullptr, old.data()) == 0 && old == mask);
    Require(pthread_sigmask_nid_postfix(2, mask.data(), old.data()) == 0 && old == mask);
    Require(received == 15 && deliveries == 1);
    received = 0;
    Require(pthread_sigmask_nid_postfix(2, mask.data(), nullptr) == 0 && received == 0);
#ifdef _WIN32
    Require(sigfillset_nid_postfix(mask.data()) == 0);
    Require(pthread_sigmask_nid_postfix(3, mask.data(), mask.data()) == 0);
    Require(mask == std::array<std::uint32_t, 4>{});
    Require(pthread_sigmask_nid_postfix(0, nullptr, old.data()) == 0);
    Require(old[0] == (UINT32_MAX & ~((1u << 8) | (1u << 16))));
    Require(old[1] == UINT32_MAX && old[2] == UINT32_MAX && old[3] == UINT32_MAX);
    Require(pthread_sigmask_nid_postfix(3, mask.data(), nullptr) == 0);
#endif
    Require(raise_nid_postfix(15) == 0 && received == 15);
    received = 0;
    Require(raise_nid_postfix(15) == 0 && received == 15);
    Require(signal_nid_postfix(15, ignore) != invalid);
    received = 0;
    Require(raise_nid_postfix(15) == 0 && received == 0);
    Require(signal_nid_postfix(15, nullptr) == ignore);
    Require(raise_nid_postfix(100) == -1 && *__error_nid_postfix() == 22);
}
