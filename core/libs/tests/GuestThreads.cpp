#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
extern "C" {
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
