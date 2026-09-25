#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
extern "C" {
int APS5_VABI pthread_getcpuclockid_nid_postfix(Pthread, int*);
int APS5_VABI clock_gettime_nid_postfix(int, KernelTimespec*);
int APS5_VABI clock_getres_nid_postfix(int, KernelTimespec*);
int* APS5_VABI __error_nid_postfix();
void APS5_VABI pthread_set_name_np_nid_postfix(Pthread, const char*);
int APS5_VABI pthread_rename_np_nid_postfix(Pthread, const char*);
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
static void CheckName() {
#ifdef _WIN32
    using GetDescription = HRESULT (WINAPI*)(HANDLE, PWSTR*);
    const auto getDescription = reinterpret_cast<GetDescription>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetThreadDescription"));
    Require(getDescription != nullptr);
    PWSTR text = nullptr;
    Require(SUCCEEDED(getDescription(GetCurrentThread(), &text)));
    Require(std::wcscmp(text, L"guest-worker") == 0);
    LocalFree(text);
#else
    char text[16]{};
    Require(pthread_getname_np(pthread_self(), text, sizeof(text)) == 0);
    Require(std::strcmp(text, "guest-worker") == 0);
#endif
}
struct State {
    int clockId = 0;
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
    int ownClock = 0;
    Require(pthread_getcpuclockid_nid_postfix(state.observed, &ownClock) == 0 && ownClock == state.clockId);
    CheckName();
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
    int mainClock = 0;
    Require(pthread_getcpuclockid_nid_postfix(mainThread, &mainClock) == 0 && mainClock < 0);
    Require(pthread_getcpuclockid_nid_postfix(nullptr, &mainClock) == 3 && mainClock < 0);
    Require(pthread_getcpuclockid_nid_postfix(mainThread, nullptr) == 22);
    KernelTimespec before{}, after{}, resolution{};
    Require(clock_gettime_nid_postfix(mainClock, &before) == 0);
    const auto until = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    while (std::chrono::steady_clock::now() < until) std::atomic_signal_fence(std::memory_order_seq_cst);
    Require(clock_gettime_nid_postfix(mainClock, &after) == 0);
    Require(after.tv_sec > before.tv_sec || (after.tv_sec == before.tv_sec && after.tv_nsec > before.tv_nsec));
    Require(clock_getres_nid_postfix(mainClock, &resolution) == 0);
    Require(resolution.tv_sec >= 0 && resolution.tv_nsec >= 0 && resolution.tv_nsec < 1000000000);
    Require(resolution.tv_sec || resolution.tv_nsec);
    Require(clock_getres_nid_postfix(mainClock, nullptr) == 0);
    Require(clock_gettime_nid_postfix(14, &before) == 0);
    Require(before.tv_sec > after.tv_sec || (before.tv_sec == after.tv_sec && before.tv_nsec >= after.tv_nsec));
    Require(clock_getres_nid_postfix(14, &resolution) == 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    Require(clock_gettime_nid_postfix(mainClock, &after) == 0);
    const auto sleepCpu = (after.tv_sec - before.tv_sec) * 1000000000LL + after.tv_nsec - before.tv_nsec;
    Require(sleepCpu >= 0 && sleepCpu < 100000000);
    Require(mainThread && mainThread == scePthreadSelf());
    Require(pthread_self_nid_postfix() == mainThread);
    pthread_set_name_np_nid_postfix(mainThread, "guest-worker");
    CheckName();
    Require(pthread_rename_np_nid_postfix(nullptr, "invalid") == 22);
    Require(pthread_rename_np_nid_postfix(mainThread, nullptr) == 22);
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
        pthread_set_name_np_nid_postfix(state.created, "guest-worker");
        Require(pthread_getcpuclockid_nid_postfix(state.created, &state.clockId) == 0);
        Require(state.clockId != mainClock);
        Require(clock_gettime_nid_postfix(state.clockId, &after) == 0);
    }
    for (std::size_t first = 0; first < states.size(); ++first)
        for (std::size_t second = first + 1; second < states.size(); ++second)
            Require(!pthread_equal_nid_postfix(states[first].created, states[second].created));
    for (auto& state : states) state.ready.store(true, std::memory_order_release);
    for (auto& state : states) {
        void* result = nullptr;
        Require(pthread_join_nid_postfix(state.created, &result) == 0);
        Require(result == &state.result && state.result == 42);
        Require(clock_gettime_nid_postfix(state.clockId, &after) == -1 && *__error_nid_postfix() == 22);
    }
}
