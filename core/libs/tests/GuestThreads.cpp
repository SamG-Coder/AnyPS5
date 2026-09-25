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
int APS5_VABI pthread_setcancelstate_nid_postfix(int, int*);
int APS5_VABI pthread_setcanceltype_nid_postfix(int, int*);
int APS5_VABI scePthreadSetcancelstate(int, int*);
int APS5_VABI scePthreadSetcanceltype(int, int*);
int APS5_VABI pthread_getcpuclockid_nid_postfix(Pthread, int*);
int APS5_VABI clock_gettime_nid_postfix(int, KernelTimespec*);
int APS5_VABI clock_getres_nid_postfix(int, KernelTimespec*);
int* APS5_VABI __error_nid_postfix();
void APS5_VABI pthread_set_name_np_nid_postfix(Pthread, const char*);
int APS5_VABI pthread_rename_np_nid_postfix(Pthread, const char*);
int APS5_VABI pthread_create_nid_postfix(Pthread*, const PthreadAttr*, PthreadEntry, void*);
int APS5_VABI pthread_join_nid_postfix(Pthread, void**);
int APS5_VABI pthread_key_create_nid_postfix(PthreadKey*, pthread_key_destructor_func_t);
int APS5_VABI pthread_key_delete_nid_postfix(PthreadKey);
void* APS5_VABI pthread_getspecific_nid_postfix(PthreadKey);
int APS5_VABI pthread_setspecific_nid_postfix(PthreadKey, void*);
int APS5_VABI scePthreadKeyCreate(PthreadKey*, pthread_key_destructor_func_t);
int APS5_VABI scePthreadKeyDelete(PthreadKey);
Pthread APS5_VABI pthread_self_nid_postfix();
int APS5_VABI pthread_equal_nid_postfix(Pthread, Pthread);
void APS5_VABI pthread_yield_nid_postfix();
int APS5_VABI sched_yield_nid_postfix();
Pthread APS5_VABI scePthreadSelf();
int APS5_VABI scePthreadEqual(Pthread, Pthread);
}
static void Require(bool value) { if (!value) std::abort(); }
static void* APS5_VABI CheckCancellationDefaults(void*) {
    int previous = -1;
    Require(pthread_setcancelstate_nid_postfix(1, &previous) == 0 && previous == 0);
    Require(pthread_setcanceltype_nid_postfix(2, &previous) == 0 && previous == 0);
    Require(scePthreadSetcancelstate(0, &previous) == 0 && previous == 1);
    Require(scePthreadSetcanceltype(0, &previous) == 0 && previous == 2);
    return nullptr;
}
static void CheckCancellationSettings() {
    struct Guarded { int before; int previous; int after; } value{123, -1, 456};
    *__error_nid_postfix() = 13;
    Require(pthread_setcancelstate_nid_postfix(1, &value.previous) == 0 && value.previous == 0);
    Require(pthread_setcanceltype_nid_postfix(2, &value.previous) == 0 && value.previous == 0);
    Require(*__error_nid_postfix() == 13);
    value.previous = -1;
    Require(pthread_setcancelstate_nid_postfix(2, &value.previous) == 22 && value.previous == -1);
    Require(pthread_setcanceltype_nid_postfix(1, &value.previous) == 22 && value.previous == -1);
    Require(scePthreadSetcanceltype(-1, &value.previous) == static_cast<int>(0x80020016u));
    Require(value.previous == -1 && value.before == 123 && value.after == 456 && *__error_nid_postfix() == 13);
    Pthread worker = nullptr;
    Require(pthread_create_nid_postfix(&worker, nullptr, CheckCancellationDefaults, nullptr) == 0);
    Require(pthread_join_nid_postfix(worker, nullptr) == 0);
    Require(scePthreadSetcancelstate(0, &value.previous) == 0 && value.previous == 1);
    Require(scePthreadSetcanceltype(0, &value.previous) == 0 && value.previous == 2);
    Require(pthread_setcancelstate_nid_postfix(0, nullptr) == 0);
    Require(pthread_setcanceltype_nid_postfix(0, nullptr) == 0);
}
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
static PthreadKey rearmKey = -1;
static int rearmCount = 0;
static void APS5_VABI Rearm(void* value) {
    ++rearmCount;
    if (rearmCount == 1) Require(pthread_setspecific_nid_postfix(rearmKey, value) == 0);
}
static int spinCount = 0;
static void APS5_VABI Spin(void* value) {
    ++spinCount;
    Require(pthread_setspecific_nid_postfix(rearmKey, value) == 0);
}
static void* APS5_VABI StoreKey(void* arg) {
    const auto key = *static_cast<PthreadKey*>(arg);
    Require(pthread_getspecific_nid_postfix(key) == nullptr);
    Require(pthread_setspecific_nid_postfix(key, arg) == 0);
    Require(pthread_getspecific_nid_postfix(key) == arg);
    return arg;
}
static void CheckThreadKeys() {
    *__error_nid_postfix() = 13;
    PthreadKey missing = 7;
    Require(pthread_key_create_nid_postfix(nullptr, nullptr) == 22 && *__error_nid_postfix() == 13);
    Require(scePthreadKeyCreate(nullptr, nullptr) == static_cast<int>(0x80020016u));
    Require(pthread_getspecific_nid_postfix(-1) == nullptr);
    Require(pthread_setspecific_nid_postfix(-1, &missing) == 22);
    PthreadKey key = -1;
    Require(pthread_key_create_nid_postfix(&key, nullptr) == 0 && key >= 0);
    Require(pthread_getspecific_nid_postfix(key) == nullptr);
    int marker = 0;
    Require(pthread_setspecific_nid_postfix(key, &marker) == 0);
    Require(pthread_getspecific_nid_postfix(key) == &marker);
    Pthread worker = nullptr;
    Require(pthread_create_nid_postfix(&worker, nullptr, StoreKey, &key) == 0);
    void* workerValue = nullptr;
    Require(pthread_join_nid_postfix(worker, &workerValue) == 0 && workerValue == &key);
    Require(pthread_getspecific_nid_postfix(key) == &marker);
    Require(pthread_setspecific_nid_postfix(key, nullptr) == 0);
    Require(pthread_key_delete_nid_postfix(key) == 0);
    Require(pthread_getspecific_nid_postfix(key) == nullptr);
    Require(pthread_setspecific_nid_postfix(key, &marker) == 22);
    Require(pthread_key_delete_nid_postfix(key) == 22);
    Require(scePthreadKeyDelete(key) == static_cast<int>(0x80020016u));
    PthreadKey reused = -1;
    Require(pthread_key_create_nid_postfix(&reused, nullptr) == 0);
    Require(pthread_getspecific_nid_postfix(reused) == nullptr);
    Require(pthread_key_delete_nid_postfix(reused) == 0);
    rearmCount = 0;
    Require(pthread_key_create_nid_postfix(&rearmKey, reinterpret_cast<pthread_key_destructor_func_t>(Rearm)) == 0);
    Require(pthread_create_nid_postfix(&worker, nullptr, StoreKey, &rearmKey) == 0);
    Require(pthread_join_nid_postfix(worker, nullptr) == 0 && rearmCount == 2);
    spinCount = 0;
    Require(pthread_key_delete_nid_postfix(rearmKey) == 0);
    Require(pthread_key_create_nid_postfix(&rearmKey, reinterpret_cast<pthread_key_destructor_func_t>(Spin)) == 0);
    Require(pthread_create_nid_postfix(&worker, nullptr, StoreKey, &rearmKey) == 0);
    Require(pthread_join_nid_postfix(worker, nullptr) == 0 && spinCount == 4);
    Require(pthread_key_delete_nid_postfix(rearmKey) == 0);
    std::array<PthreadKey, 256> keys{};
    for (auto& created : keys) Require(pthread_key_create_nid_postfix(&created, nullptr) == 0);
    PthreadKey overflow = -1;
    Require(pthread_key_create_nid_postfix(&overflow, nullptr) == 35 && overflow == -1);
    Require(pthread_key_delete_nid_postfix(keys[0]) == 0);
    Require(pthread_key_create_nid_postfix(&overflow, nullptr) == 0);
    Require(pthread_getspecific_nid_postfix(overflow) == nullptr);
    Require(pthread_key_delete_nid_postfix(overflow) == 0);
    for (std::size_t index = 1; index < keys.size(); ++index)
        Require(pthread_key_delete_nid_postfix(keys[index]) == 0);
    Require(*__error_nid_postfix() == 13);
}
int main() {
    CheckCancellationSettings();
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
    CheckThreadKeys();
}
