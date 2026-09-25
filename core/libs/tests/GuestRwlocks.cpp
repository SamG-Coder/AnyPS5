#include "SceTypes.hpp"
#include <array>
#include <atomic>
#include <cstdlib>
#include <thread>
extern "C" {
int APS5_VABI pthread_rwlock_init_nid_postfix(PthreadRwlock*, const PthreadRwlockattr*);
int APS5_VABI pthread_rwlock_destroy_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_rwlock_rdlock_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_rwlock_wrlock_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_rwlock_tryrdlock_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_rwlock_trywrlock_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_rwlock_unlock_nid_postfix(PthreadRwlock*);
int APS5_VABI pthread_create_nid_postfix(Pthread*, const PthreadAttr*, PthreadEntry, void*);
int APS5_VABI pthread_join_nid_postfix(Pthread, void**);
}
static void Require(bool value) { if (!value) std::abort(); }
static PthreadRwlock lock = nullptr;
static std::atomic<bool> readShared{false};
static unsigned first = 0, second = 0;
static void* APS5_VABI ReaderThenWriter(void*) {
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 1);
    Require(pthread_rwlock_tryrdlock_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_trywrlock_nid_postfix(&lock) == 16);
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    readShared.store(true, std::memory_order_release);
    Require(pthread_rwlock_wrlock_nid_postfix(&lock) == 0);
    first = second = 1;
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    return nullptr;
}
static void* APS5_VABI Worker(void* argument) {
    const bool write = *static_cast<bool*>(argument);
    for (unsigned iteration = 0; iteration < 500; ++iteration) {
        Require((write ? pthread_rwlock_wrlock_nid_postfix(&lock) : pthread_rwlock_rdlock_nid_postfix(&lock)) == 0);
        Require(first == second);
        if (write) { ++first; std::this_thread::yield(); ++second; }
        Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    }
    return nullptr;
}
int main() {
    Require(pthread_rwlock_rdlock_nid_postfix(nullptr) == 22);
    Require(pthread_rwlock_destroy_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_rdlock_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_rdlock_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_destroy_nid_postfix(&lock) == 16);
    Require(pthread_rwlock_wrlock_nid_postfix(&lock) == 11);
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    Pthread child = nullptr;
    Require(pthread_create_nid_postfix(&child, nullptr, ReaderThenWriter, nullptr) == 0);
    while (!readShared.load(std::memory_order_acquire)) std::this_thread::yield();
    Require(first == 0 && second == 0);
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    Require(pthread_join_nid_postfix(child, nullptr) == 0);
    Require(first == 1 && second == 1);
    Require(pthread_rwlock_wrlock_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_rdlock_nid_postfix(&lock) == 11);
    Require(pthread_rwlock_tryrdlock_nid_postfix(&lock) == 16);
    Require(pthread_rwlock_destroy_nid_postfix(&lock) == 16);
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 0);
    Require(pthread_rwlock_destroy_nid_postfix(&lock) == 0 && !lock);
    Require(pthread_rwlock_init_nid_postfix(&lock, nullptr) == 0);
    std::array<Pthread, 6> threads{};
    std::array<bool, 6> writers{true, false, true, false, true, false};
    for (unsigned index = 0; index < threads.size(); ++index)
        Require(pthread_create_nid_postfix(&threads[index], nullptr, Worker, &writers[index]) == 0);
    for (auto thread : threads) Require(pthread_join_nid_postfix(thread, nullptr) == 0);
    Require(first == 1501 && second == 1501);
    Require(pthread_rwlock_unlock_nid_postfix(&lock) == 1);
    Require(pthread_rwlock_destroy_nid_postfix(&lock) == 0);
}
