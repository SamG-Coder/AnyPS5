#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <utility>
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
int APS5_VABI pthread_getschedparam_nid_postfix(Pthread, int*, KernelSchedParam*);
int APS5_VABI pthread_setschedparam_nid_postfix(Pthread, int, const KernelSchedParam*);
int APS5_VABI pthread_key_create_nid_postfix(PthreadKey*, pthread_key_destructor_func_t);
int APS5_VABI pthread_key_delete_nid_postfix(PthreadKey);
void* APS5_VABI pthread_getspecific_nid_postfix(PthreadKey);
int APS5_VABI pthread_setspecific_nid_postfix(PthreadKey, void*);
int APS5_VABI scePthreadKeyCreate(PthreadKey*, pthread_key_destructor_func_t);
int APS5_VABI scePthreadKeyDelete(PthreadKey);
int APS5_VABI pthread_mutex_init_nid_postfix(PthreadMutex*, const PthreadMutexattr*);
int APS5_VABI pthread_mutex_destroy_nid_postfix(PthreadMutex*);
int APS5_VABI pthread_mutex_lock_nid_postfix(PthreadMutex*);
int APS5_VABI pthread_mutex_trylock_nid_postfix(PthreadMutex*);
int APS5_VABI pthread_mutex_timedlock_nid_postfix(PthreadMutex*, const KernelTimespec*);
int APS5_VABI pthread_mutex_unlock_nid_postfix(PthreadMutex*);
int APS5_VABI pthread_mutexattr_init_nid_postfix(PthreadMutexattr*);
int APS5_VABI pthread_mutexattr_destroy_nid_postfix(PthreadMutexattr*);
int APS5_VABI pthread_mutexattr_settype_nid_postfix(PthreadMutexattr*, int);
int APS5_VABI pthread_mutexattr_setprotocol_nid_postfix(PthreadMutexattr*, int);
int APS5_VABI clock_gettime_nid_postfix(int, KernelTimespec*);
int APS5_VABI sem_init_nid_postfix(void*, int, unsigned);
int APS5_VABI sem_destroy_nid_postfix(void*);
int APS5_VABI sem_wait_nid_postfix(void*);
int APS5_VABI sem_trywait_nid_postfix(void*);
int APS5_VABI sem_post_nid_postfix(void*);
int APS5_VABI sem_getvalue_nid_postfix(void*, int*);
int APS5_VABI sem_timedwait_nid_postfix(void*, const KernelTimespec*);
int APS5_VABI scePthreadSemInit(void*, int, unsigned, const char*);
int APS5_VABI pthread_attr_init_nid_postfix(PthreadAttr*);
int APS5_VABI pthread_attr_destroy_nid_postfix(PthreadAttr*);
int APS5_VABI pthread_attr_getdetachstate_nid_postfix(const PthreadAttr*, int*);
int APS5_VABI pthread_attr_getguardsize_nid_postfix(const PthreadAttr*, std::size_t*);
int APS5_VABI pthread_attr_getinheritsched_nid_postfix(const PthreadAttr*, int*);
int APS5_VABI pthread_attr_getschedparam_nid_postfix(const PthreadAttr*, KernelSchedParam*);
int APS5_VABI pthread_attr_getschedpolicy_nid_postfix(const PthreadAttr*, int*);
int APS5_VABI pthread_attr_getstack_nid_postfix(const PthreadAttr*, void**, std::size_t*);
int APS5_VABI pthread_attr_getstacksize_nid_postfix(const PthreadAttr*, std::size_t*);
int APS5_VABI pthread_attr_get_np_nid_postfix(Pthread, PthreadAttr*);
int APS5_VABI pthread_attr_setdetachstate_nid_postfix(PthreadAttr*, int);
int APS5_VABI pthread_attr_setguardsize_nid_postfix(PthreadAttr*, std::size_t);
int APS5_VABI pthread_attr_setinheritsched_nid_postfix(PthreadAttr*, int);
int APS5_VABI pthread_attr_setschedparam_nid_postfix(PthreadAttr*, const KernelSchedParam*);
int APS5_VABI pthread_attr_setschedpolicy_nid_postfix(PthreadAttr*, int);
int APS5_VABI pthread_attr_setstacksize_nid_postfix(PthreadAttr*, std::size_t);
int APS5_VABI scePthreadAttrSetstack(PthreadAttr*, void*, std::size_t);
Pthread APS5_VABI pthread_self_nid_postfix();
int APS5_VABI pthread_equal_nid_postfix(Pthread, Pthread);
void APS5_VABI pthread_yield_nid_postfix();
int APS5_VABI sched_yield_nid_postfix();
Pthread APS5_VABI scePthreadSelf();
int APS5_VABI scePthreadEqual(Pthread, Pthread);
}
static void RequireAt(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "thread check failed at line %d\n", line);
        std::abort();
    }
}
#define Require(value) RequireAt((value), __LINE__)
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
static void* APS5_VABI WaitForPost(void* arg) {
    return reinterpret_cast<void*>(static_cast<std::intptr_t>(sem_wait_nid_postfix(arg) == 0));
}
static void CheckSemaphores() {
    *__error_nid_postfix() = 13;
    alignas(8) unsigned char storage[16]{};
    Require(sem_init_nid_postfix(nullptr, 0, 0) == -1 && *__error_nid_postfix() == 22);
    Require(sem_init_nid_postfix(storage, 2, 0) == -1 && *__error_nid_postfix() == 22);
    const std::array<unsigned char, 16> untouched{};
    Require(sem_init_nid_postfix(storage, 1, 0) == -1 && *__error_nid_postfix() == 45);
    Require(std::memcmp(storage, untouched.data(), sizeof(storage)) == 0);
    *__error_nid_postfix() = 13;
    Require(scePthreadSemInit(storage, 1, 0, "shared") == static_cast<int>(0x8002002du));
    Require(*__error_nid_postfix() == 13);
    Require(std::memcmp(storage, untouched.data(), sizeof(storage)) == 0);
    Require(sem_init_nid_postfix(storage, 0, 1u << 31) == -1 && *__error_nid_postfix() == 22);
    Require(sem_init_nid_postfix(storage, 0, 0) == 0 && *__error_nid_postfix() == 22);
    int value = -1;
    Require(sem_getvalue_nid_postfix(storage, &value) == 0 && value == 0);
    Require(sem_trywait_nid_postfix(storage) == -1 && *__error_nid_postfix() == 35);
    Require(sem_post_nid_postfix(storage) == 0);
    Require(sem_getvalue_nid_postfix(storage, &value) == 0 && value == 1);
    Require(sem_trywait_nid_postfix(storage) == 0);
    Require(sem_post_nid_postfix(storage) == 0);
    Pthread worker = nullptr;
    Require(pthread_create_nid_postfix(&worker, nullptr, WaitForPost, storage) == 0);
    void* woke = nullptr;
    Require(pthread_join_nid_postfix(worker, &woke) == 0 && woke == reinterpret_cast<void*>(1));
    KernelTimespec past{};
    Require(clock_gettime_nid_postfix(0, &past) == 0);
    past.tv_sec -= 1;
    Require(sem_timedwait_nid_postfix(storage, &past) == -1 && *__error_nid_postfix() == 60);
    Require(sem_destroy_nid_postfix(storage) == 0);
    Require(sem_post_nid_postfix(storage) == -1 && *__error_nid_postfix() == 22);
    Require(scePthreadSemInit(storage, 0, 1, "guest") == 0);
    Require(*__error_nid_postfix() == 22);
    Require(sem_trywait_nid_postfix(storage) == 0);
    Require(sem_destroy_nid_postfix(storage) == 0);
    Require(*__error_nid_postfix() == 22);
}
static void* APS5_VABI TryHeldMutex(void* arg) {
    return reinterpret_cast<void*>(static_cast<std::intptr_t>(
        pthread_mutex_trylock_nid_postfix(static_cast<PthreadMutex*>(arg))));
}
static void* APS5_VABI TimeHeldMutex(void* arg) {
    auto* state = static_cast<std::pair<PthreadMutex*, KernelTimespec>*>(arg);
    return reinterpret_cast<void*>(static_cast<std::intptr_t>(
        pthread_mutex_timedlock_nid_postfix(state->first, &state->second)));
}
static void CheckMutexes() {
    *__error_nid_postfix() = 13;
    PthreadMutexattr attr = nullptr;
    Require(pthread_mutexattr_init_nid_postfix(nullptr) == 22);
    Require(pthread_mutexattr_init_nid_postfix(&attr) == 0 && attr);
    Require(pthread_mutexattr_settype_nid_postfix(&attr, 4) == 22);
    Require(pthread_mutexattr_settype_nid_postfix(&attr, 2) == 0);
    Require(pthread_mutexattr_setprotocol_nid_postfix(&attr, 0) == 0);
    Require(pthread_mutexattr_setprotocol_nid_postfix(&attr, 1) == 45);
    Require(pthread_mutexattr_setprotocol_nid_postfix(&attr, 9) == 22);
    PthreadMutex mutex = nullptr;
    Require(pthread_mutex_init_nid_postfix(nullptr, nullptr) == 22);
    Require(pthread_mutex_init_nid_postfix(&mutex, &attr) == 0 && mutex);
    Require(pthread_mutex_lock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_lock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_unlock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_unlock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_unlock_nid_postfix(&mutex) == 1);
    Require(pthread_mutex_destroy_nid_postfix(&mutex) == 0 && !mutex);
    Require(pthread_mutexattr_destroy_nid_postfix(&attr) == 0 && !attr);
    Require(pthread_mutex_init_nid_postfix(&mutex, nullptr) == 0);
    Require(pthread_mutex_lock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_trylock_nid_postfix(&mutex) == 16);
    Pthread worker = nullptr;
    Require(pthread_create_nid_postfix(&worker, nullptr, TryHeldMutex, &mutex) == 0);
    void* busy = nullptr;
    Require(pthread_join_nid_postfix(worker, &busy) == 0 && busy == reinterpret_cast<void*>(16));
    KernelTimespec past{};
    Require(clock_gettime_nid_postfix(0, &past) == 0 && past.tv_sec > 0);
    past.tv_sec -= 1;
    std::pair<PthreadMutex*, KernelTimespec> held{&mutex, past};
    Require(pthread_create_nid_postfix(&worker, nullptr, TimeHeldMutex, &held) == 0);
    void* timed = nullptr;
    Require(pthread_join_nid_postfix(worker, &timed) == 0 && timed == reinterpret_cast<void*>(60));
    Require(pthread_mutex_unlock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_timedlock_nid_postfix(&mutex, &past) == 0);
    Require(pthread_mutex_unlock_nid_postfix(&mutex) == 0);
    Require(pthread_mutex_timedlock_nid_postfix(&mutex, nullptr) == 22);
    past.tv_nsec = 1000000000;
    Require(pthread_mutex_timedlock_nid_postfix(&mutex, &past) == 22);
    Require(pthread_mutex_destroy_nid_postfix(&mutex) == 0 && !mutex);
    PthreadMutex staticMutex = nullptr;
    Require(pthread_mutex_lock_nid_postfix(&staticMutex) == 0 && staticMutex);
    Require(pthread_mutex_trylock_nid_postfix(&staticMutex) == 16);
    Require(pthread_mutex_unlock_nid_postfix(&staticMutex) == 0);
    Require(pthread_mutex_destroy_nid_postfix(&staticMutex) == 0 && !staticMutex);
    for (int round = 0; round < 20; ++round) {
        PthreadMutex shared = nullptr;
        std::atomic<int> ready = 0;
        std::atomic<bool> start = false;
        int counter = 0;
        std::array<std::thread, 8> workers;
        for (auto& worker : workers) worker = std::thread([&] {
            ready.fetch_add(1);
            while (!start.load()) std::this_thread::yield();
            for (int iteration = 0; iteration < 100; ++iteration) {
                Require(pthread_mutex_lock_nid_postfix(&shared) == 0);
                ++counter;
                Require(pthread_mutex_unlock_nid_postfix(&shared) == 0);
            }
        });
        while (ready.load() != 8) std::this_thread::yield();
        start.store(true);
        for (auto& worker : workers) worker.join();
        Require(counter == 800);
        Require(pthread_mutex_destroy_nid_postfix(&shared) == 0 && !shared);
    }
    Require(pthread_mutex_lock_nid_postfix(nullptr) == 22);
    Require(*__error_nid_postfix() == 13);
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
static std::atomic<int> attributeEntries{0};
static void* APS5_VABI CountAttributeThread(void*) {
    attributeEntries.fetch_add(1, std::memory_order_release);
    return nullptr;
}
static void* APS5_VABI HoldAttributeThread(void* arg) {
    auto* flag = static_cast<std::atomic<int>*>(arg);
    while (flag->load(std::memory_order_acquire) == 0) pthread_yield_nid_postfix();
    flag->store(2, std::memory_order_release);
    return nullptr;
}
static void ExerciseAttributes() {
    for (int index = 0; index < 32; ++index) {
        PthreadAttr attr = nullptr;
        if (pthread_attr_init_nid_postfix(&attr) != 0 || !attr) std::abort();
        if (pthread_attr_setdetachstate_nid_postfix(&attr, 0) != 0) std::abort();
        if (pthread_attr_setstacksize_nid_postfix(&attr, 1u << 20) != 0) std::abort();
        if (pthread_attr_destroy_nid_postfix(&attr) != 0 || attr) std::abort();
    }
}
static void CheckAttributes() {
    *__error_nid_postfix() = 13;
    PthreadAttr attr = nullptr;
    Require(pthread_attr_init_nid_postfix(nullptr) == 22 && *__error_nid_postfix() == 13);
    Require(pthread_attr_init_nid_postfix(&attr) == 0 && attr && *__error_nid_postfix() == 13);
    int state = -1;
    std::size_t stack = 0;
    std::size_t guard = 1;
    int policy = -1;
    int inherit = -1;
    void* address = reinterpret_cast<void*>(1);
    KernelSchedParam param{-1};
    Require(pthread_attr_getdetachstate_nid_postfix(&attr, &state) == 0 && state == 0);
    Require(pthread_attr_getstacksize_nid_postfix(&attr, &stack) == 0 && stack == (1u << 20));
    Require(pthread_attr_getguardsize_nid_postfix(&attr, &guard) == 0 && guard == 0);
    Require(pthread_attr_getschedpolicy_nid_postfix(&attr, &policy) == 0 && policy == 1);
    Require(pthread_attr_getinheritsched_nid_postfix(&attr, &inherit) == 0 && inherit == 4);
    Require(pthread_attr_getschedparam_nid_postfix(&attr, &param) == 0 && param.sched_priority == 700);
    Require(pthread_attr_getstack_nid_postfix(&attr, &address, &stack) == 0 && address == nullptr && stack == (1u << 20));
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 2) == 22);
    Require(pthread_attr_getdetachstate_nid_postfix(&attr, &state) == 0 && state == 0);
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 1) == 0);
    Require(pthread_attr_getdetachstate_nid_postfix(&attr, &state) == 0 && state == 1);
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 0) == 0);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 2047) == 22);
    Require(pthread_attr_getstacksize_nid_postfix(&attr, &stack) == 0 && stack == (1u << 20));
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 2048) == 0);
    Require(pthread_attr_getstacksize_nid_postfix(&attr, &stack) == 0 && stack == 2048);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 8u << 20) == 0);
    Require(pthread_attr_setschedpolicy_nid_postfix(&attr, 4) == 22);
    Require(pthread_attr_getschedpolicy_nid_postfix(&attr, &policy) == 0 && policy == 1);
    Require(pthread_attr_setschedpolicy_nid_postfix(&attr, 3) == 0);
    Require(pthread_attr_getschedpolicy_nid_postfix(&attr, &policy) == 0 && policy == 3);
    Require(pthread_attr_setinheritsched_nid_postfix(&attr, 1) == 22);
    Require(pthread_attr_getinheritsched_nid_postfix(&attr, &inherit) == 0 && inherit == 4);
    Require(pthread_attr_setinheritsched_nid_postfix(&attr, 0) == 0);
    Require(pthread_attr_getinheritsched_nid_postfix(&attr, &inherit) == 0 && inherit == 0);
    Require(pthread_attr_setguardsize_nid_postfix(&attr, 0x4000) == 45);
    Require(pthread_attr_getguardsize_nid_postfix(&attr, &guard) == 0 && guard == 0);
    Require(pthread_attr_setguardsize_nid_postfix(&attr, 0) == 0);
    Require(pthread_attr_setschedparam_nid_postfix(&attr, nullptr) == 22);
    KernelSchedParam custom{123};
    Require(pthread_attr_setschedparam_nid_postfix(&attr, &custom) == 0);
    Require(pthread_attr_getschedparam_nid_postfix(&attr, &param) == 0 && param.sched_priority == 123);
    Require(pthread_attr_getdetachstate_nid_postfix(&attr, nullptr) == 22 && state == 1);
    Require(pthread_attr_getstack_nid_postfix(&attr, nullptr, &stack) == 22);
    Require(pthread_attr_destroy_nid_postfix(nullptr) == 22);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 0 && !attr);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 22);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 1u << 20) == 22);
    Require(*__error_nid_postfix() == 13);

    Require(pthread_attr_init_nid_postfix(&attr) == 0);
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 0) == 0);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 2048) == 0);
    Pthread thread = nullptr;
    const auto rejectedEntries = attributeEntries.load();
    Require(pthread_attr_setinheritsched_nid_postfix(&attr, 0) == 0);
    Require(pthread_create_nid_postfix(&thread, &attr, CountAttributeThread, nullptr) == 45 && !thread);
    Require(pthread_attr_setinheritsched_nid_postfix(&attr, 4) == 0);
#ifdef _WIN32
    Require(pthread_attr_setstacksize_nid_postfix(&attr, SIZE_MAX) == 0);
    Require(pthread_create_nid_postfix(&thread, &attr, CountAttributeThread, nullptr) == 22 && !thread);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 2048) == 0);
#endif
    std::array<unsigned char, 2048> suppliedStack{};
    Require(scePthreadAttrSetstack(&attr, suppliedStack.data(), suppliedStack.size()) == 0);
    Require(pthread_create_nid_postfix(&thread, &attr, CountAttributeThread, nullptr) == 45 && !thread);
    Require(attributeEntries.load() == rejectedEntries);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 0);
    Require(pthread_attr_init_nid_postfix(&attr) == 0);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 2048) == 0);
    const auto before = attributeEntries.load();
    Require(pthread_create_nid_postfix(&thread, &attr, CountAttributeThread, nullptr) == 0);
    PthreadAttr queried = nullptr;
    Require(pthread_attr_init_nid_postfix(&queried) == 0);
    Require(pthread_attr_get_np_nid_postfix(nullptr, &queried) == 22);
    Require(pthread_attr_get_np_nid_postfix(thread, nullptr) == 22);
    Require(pthread_attr_get_np_nid_postfix(thread, &queried) == 0);
    Require(pthread_attr_getstacksize_nid_postfix(&queried, &stack) == 0 && stack == 2048);
    Require(pthread_attr_getdetachstate_nid_postfix(&queried, &state) == 0 && state == 0);
    Require(pthread_attr_getguardsize_nid_postfix(&queried, &guard) == 0 && guard == 0);
    Require(pthread_join_nid_postfix(thread, nullptr) == 0);
    Require(attributeEntries.load() == before + 1);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 0);
    Require(pthread_attr_destroy_nid_postfix(&queried) == 0);

    std::atomic<int> hold{0};
    Require(pthread_attr_init_nid_postfix(&attr) == 0);
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 1) == 0);
    Require(pthread_attr_setstacksize_nid_postfix(&attr, 8u << 20) == 0);
    Require(pthread_create_nid_postfix(&thread, &attr, HoldAttributeThread, &hold) == 0);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 0);
    Require(pthread_attr_init_nid_postfix(&queried) == 0);
    Require(pthread_attr_get_np_nid_postfix(thread, &queried) == 0);
    Require(pthread_attr_getdetachstate_nid_postfix(&queried, &state) == 0 && state == 1);
    Require(pthread_attr_getstacksize_nid_postfix(&queried, &stack) == 0 && stack == (8u << 20));
    Require(pthread_join_nid_postfix(thread, nullptr) == 22);
    hold.store(1, std::memory_order_release);
    while (hold.load(std::memory_order_acquire) != 2) pthread_yield_nid_postfix();
    Require(pthread_attr_destroy_nid_postfix(&queried) == 0);

    Require(pthread_attr_init_nid_postfix(&attr) == 0);
    Require(pthread_attr_setdetachstate_nid_postfix(&attr, 0) == 0);
    Pthread first = nullptr;
    Pthread second = nullptr;
    Require(pthread_create_nid_postfix(&first, &attr, CountAttributeThread, nullptr) == 0);
    Require(pthread_create_nid_postfix(&second, &attr, CountAttributeThread, nullptr) == 0);
    Require(pthread_join_nid_postfix(first, nullptr) == 0);
    Require(pthread_join_nid_postfix(second, nullptr) == 0);
    Require(attributeEntries.load() == before + 3);
    Require(pthread_attr_destroy_nid_postfix(&attr) == 0);
    std::thread left(ExerciseAttributes);
    std::thread right(ExerciseAttributes);
    left.join();
    right.join();
    Require(*__error_nid_postfix() == 13);
}
int main() {
    CheckCancellationSettings();
    const auto mainThread = pthread_self_nid_postfix();
    int schedulingPolicy = -1;
    KernelSchedParam schedulingParam{-1};
    *__error_nid_postfix() = 13;
    Require(pthread_getschedparam_nid_postfix(mainThread, &schedulingPolicy, &schedulingParam) == 45);
    Require(schedulingPolicy == -1 && schedulingParam.sched_priority == -1);
    Require(pthread_getschedparam_nid_postfix(nullptr, &schedulingPolicy, &schedulingParam) == 22);
    Require(pthread_getschedparam_nid_postfix(mainThread, nullptr, &schedulingParam) == 22);
    Require(pthread_getschedparam_nid_postfix(mainThread, &schedulingPolicy, nullptr) == 22);
    Require(pthread_setschedparam_nid_postfix(mainThread, 1, &schedulingParam) == 45);
    Require(pthread_setschedparam_nid_postfix(mainThread, 4, &schedulingParam) == 22);
    Require(pthread_setschedparam_nid_postfix(mainThread, 1, nullptr) == 22);
    Require(pthread_setschedparam_nid_postfix(nullptr, 1, &schedulingParam) == 22);
    Require(*__error_nid_postfix() == 13);
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
    CheckMutexes();
    CheckSemaphores();
    CheckAttributes();
}
