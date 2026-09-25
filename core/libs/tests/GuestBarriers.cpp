#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdlib>
extern "C" {
int APS5_VABI pthread_barrier_init_nid_postfix(void**, void* const*, unsigned);
int APS5_VABI pthread_barrier_wait_nid_postfix(void**);
int APS5_VABI pthread_barrier_destroy_nid_postfix(void**);
int APS5_VABI pthread_barrierattr_init_nid_postfix(void**);
int APS5_VABI pthread_barrierattr_destroy_nid_postfix(void**);
int APS5_VABI pthread_barrierattr_getpshared_nid_postfix(void* const*, int*);
int APS5_VABI pthread_barrierattr_setpshared_nid_postfix(void**, int);
int APS5_VABI pthread_create_nid_postfix(Pthread*, const PthreadAttr*, PthreadEntry, void*);
int APS5_VABI pthread_join_nid_postfix(Pthread, void**);
}
static void Require(bool value) { if (!value) std::abort(); }
constexpr unsigned workers = 6, rounds = 200;
static void* barrier = nullptr;
static std::array<unsigned, workers> values{};
static std::array<std::atomic<unsigned>, rounds> serial{};
static void* APS5_VABI Worker(void* argument) {
    const auto index = *static_cast<unsigned*>(argument);
    for (unsigned round = 0; round < rounds; ++round) {
        values[index] = round + 1;
        const int result = pthread_barrier_wait_nid_postfix(&barrier);
        Require(result == 0 || result == -1);
        if (result == -1) serial[round].fetch_add(1);
        for (const auto value : values) Require(value == round + 1);
        const int next = pthread_barrier_wait_nid_postfix(&barrier);
        Require(next == 0 || next == -1);
    }
    return nullptr;
}
int main() {
    Require(pthread_barrier_init_nid_postfix(nullptr, nullptr, 1) == 22);
    Require(pthread_barrier_init_nid_postfix(&barrier, nullptr, 0) == 22 && !barrier);
    Require(pthread_barrier_wait_nid_postfix(&barrier) == 22);
    Require(pthread_barrier_destroy_nid_postfix(&barrier) == 22);
    void* attr = nullptr;
    Require(pthread_barrier_init_nid_postfix(&barrier, &attr, 1) == 22);
    Require(pthread_barrierattr_init_nid_postfix(&attr) == 0);
    int shared = -1;
    Require(pthread_barrierattr_getpshared_nid_postfix(&attr, &shared) == 0 && shared == 0);
    Require(pthread_barrierattr_setpshared_nid_postfix(&attr, 1) == 45);
    Require(pthread_barrierattr_setpshared_nid_postfix(&attr, 2) == 22);
    Require(pthread_barrierattr_getpshared_nid_postfix(&attr, &shared) == 0 && shared == 0);
    Require(pthread_barrier_init_nid_postfix(&barrier, &attr, 1) == 0);
    for (unsigned round = 0; round < rounds; ++round) Require(pthread_barrier_wait_nid_postfix(&barrier) == -1);
    Require(pthread_barrier_destroy_nid_postfix(&barrier) == 0 && !barrier);
    Require(pthread_barrierattr_destroy_nid_postfix(&attr) == 0 && !attr);
    Require(pthread_barrier_init_nid_postfix(&barrier, nullptr, workers) == 0);
    std::array<Pthread, workers> threads{};
    std::array<unsigned, workers> indices{};
    for (unsigned index = 0; index < workers; ++index) {
        indices[index] = index;
        Require(pthread_create_nid_postfix(&threads[index], nullptr, Worker, &indices[index]) == 0);
    }
    for (const auto thread : threads) Require(pthread_join_nid_postfix(thread, nullptr) == 0);
    for (const auto& count : serial) Require(count.load() == 1);
    Require(pthread_barrier_destroy_nid_postfix(&barrier) == 0 && !barrier);
}
