#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdlib>
extern "C" {
int APS5_VABI pthread_create_nid_postfix(Pthread*, const PthreadAttr*, PthreadEntry, void*);
int APS5_VABI pthread_join_nid_postfix(Pthread, void**);
Pthread APS5_VABI pthread_self_nid_postfix();
int APS5_VABI pthread_equal_nid_postfix(Pthread, Pthread);
void APS5_VABI pthread_yield_nid_postfix();
int APS5_VABI sched_yield_nid_postfix();
Pthread APS5_VABI scePthreadSelf();
int APS5_VABI scePthreadEqual(Pthread, Pthread);
}
static void Require(bool value) { if (!value) std::abort(); }
struct State {
    Pthread created = nullptr;
    Pthread parent = nullptr;
    Pthread observed = nullptr;
    int result = 0;
    std::atomic<bool> ready{false};
};
static void* APS5_VABI Entry(void* argument) {
    auto& state = *static_cast<State*>(argument);
    while (!state.ready.load(std::memory_order_acquire)) {
        pthread_yield_nid_postfix();
        Require(sched_yield_nid_postfix() == 0);
    }
    state.observed = pthread_self_nid_postfix();
    Require(state.observed && state.observed == scePthreadSelf());
    Require(pthread_equal_nid_postfix(state.observed, state.created));
    Require(scePthreadEqual(state.observed, state.created));
    Require(!pthread_equal_nid_postfix(state.observed, state.parent));
    Require(pthread_join_nid_postfix(state.observed, nullptr) == 11);
    state.result = 42;
    return &state.result;
}
int main() {
    const auto mainThread = pthread_self_nid_postfix();
    Require(mainThread && mainThread == scePthreadSelf());
    Require(pthread_self_nid_postfix() == mainThread);
    Require(pthread_join_nid_postfix(mainThread, nullptr) == 11);
    Require(pthread_create_nid_postfix(nullptr, nullptr, Entry, nullptr) == 22);
    Pthread untouched = mainThread;
    Require(pthread_create_nid_postfix(&untouched, nullptr, nullptr, nullptr) == 22 && untouched == mainThread);
    PthreadAttr invalid = nullptr;
    Require(pthread_create_nid_postfix(&untouched, &invalid, Entry, nullptr) == 22 && untouched == mainThread);
    Require(pthread_join_nid_postfix(nullptr, nullptr) == 22);
    std::array<State, 8> states;
    for (auto& state : states) {
        state.parent = mainThread;
        Require(pthread_create_nid_postfix(&state.created, nullptr, Entry, &state) == 0);
    }
    for (std::size_t first = 0; first < states.size(); ++first)
        for (std::size_t second = first + 1; second < states.size(); ++second)
            Require(!pthread_equal_nid_postfix(states[first].created, states[second].created));
    for (auto& state : states) state.ready.store(true, std::memory_order_release);
    for (auto& state : states) {
        void* result = nullptr;
        Require(pthread_join_nid_postfix(state.created, &result) == 0);
        Require(result == &state.result && state.result == 42);
    }
}
