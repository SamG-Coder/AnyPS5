#include "prx/libc/include/general/VabiMacros.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <thread>

extern "C" {
void* APS5_VABI sceLibcMspaceCreate_nid_postfix(const char*, void*, std::size_t, unsigned);
int APS5_VABI sceLibcMspaceDestroy_nid_postfix(void*);
void* APS5_VABI sceLibcMspaceMalloc_nid_postfix(void*, std::size_t);
void* APS5_VABI sceLibcMspaceCalloc_nid_postfix(void*, std::size_t, std::size_t);
void* APS5_VABI sceLibcMspaceRealloc_nid_postfix(void*, void*, std::size_t);
void APS5_VABI sceLibcMspaceFree_nid_postfix(void*, void*);
int APS5_VABI sceLibcMspacePosixMemalign_nid_postfix(void*, void**, std::size_t, std::size_t);
std::size_t APS5_VABI sceLibcMspaceMallocUsableSize_nid_postfix(const void*);
}
static void Require(bool value) {
    if (!value) { std::fputs("Mspace check failed\n", stderr); std::abort(); }
}
int main() {
    alignas(16) std::array<unsigned char, 65536> storage{};
    void* arena = sceLibcMspaceCreate_nid_postfix("test", storage.data(), storage.size(), 0);
    Require(arena != nullptr);
    Require(sceLibcMspaceCreate_nid_postfix("overlap", storage.data(), storage.size(), 0) == nullptr);
    auto* first = static_cast<unsigned char*>(sceLibcMspaceCalloc_nid_postfix(arena, 32, 4));
    Require(first > storage.data() && first + 128 <= storage.data() + storage.size());
    for (int i = 0; i < 128; ++i) { Require(first[i] == 0); first[i] = static_cast<unsigned char>(i); }
    void* blocker = sceLibcMspaceMalloc_nid_postfix(arena, 128);
    auto* grown = static_cast<unsigned char*>(sceLibcMspaceRealloc_nid_postfix(arena, first, 4096));
    Require(grown && grown != first);
    for (int i = 0; i < 128; ++i) Require(grown[i] == i);
    Require(sceLibcMspaceMallocUsableSize_nid_postfix(grown) >= 4096);
    Require(sceLibcMspaceRealloc_nid_postfix(arena, grown, storage.size()) == nullptr);
    Require(grown[127] == 127);
    void* aligned = nullptr;
    Require(sceLibcMspacePosixMemalign_nid_postfix(arena, &aligned, 4096, 1024) == 0);
    Require((reinterpret_cast<std::uintptr_t>(aligned) & 4095) == 0);
    void* unchanged = aligned;
    Require(sceLibcMspacePosixMemalign_nid_postfix(arena, &unchanged, 3, 8) == 22 && unchanged == aligned);
    Require(sceLibcMspaceCalloc_nid_postfix(arena, std::numeric_limits<std::size_t>::max(), 2) == nullptr);
    sceLibcMspaceFree_nid_postfix(arena, blocker);
    sceLibcMspaceFree_nid_postfix(arena, aligned);
    Require(sceLibcMspaceRealloc_nid_postfix(arena, grown, 0) == nullptr);
    void* large = sceLibcMspaceMalloc_nid_postfix(arena, storage.size() - 256);
    Require(large != nullptr); // freeing coalesced all the fragmented blocks
    sceLibcMspaceFree_nid_postfix(arena, large);
    std::array<std::thread, 4> workers;
    for (auto& worker : workers) worker = std::thread([&] {
        for (int i = 0; i < 1000; ++i) {
            void* pointer = sceLibcMspaceMalloc_nid_postfix(arena, 97);
            Require(pointer != nullptr);
            std::memset(pointer, 42, 97);
            Require(sceLibcMspaceMallocUsableSize_nid_postfix(pointer) >= 97);
            sceLibcMspaceFree_nid_postfix(arena, pointer);
        }
    });
    for (auto& worker : workers) worker.join();
    alignas(16) std::array<unsigned char, 4096> otherStorage{};
    void* otherArena = sceLibcMspaceCreate_nid_postfix("other", otherStorage.data(), otherStorage.size(), 0);
    Require(otherArena != nullptr);
    std::array<void*, 256> pointers{};
    for (std::size_t i = 0; i < pointers.size(); ++i) {
        pointers[i] = sceLibcMspaceMalloc_nid_postfix(arena, i % 64 + 1);
        Require(pointers[i] != nullptr);
        std::memset(pointers[i], static_cast<int>(i), i % 64 + 1);
    }
    sceLibcMspaceFree_nid_postfix(otherArena, pointers[127]);
    Require(sceLibcMspaceRealloc_nid_postfix(otherArena, pointers[127], 256) == nullptr);
    Require(sceLibcMspaceMallocUsableSize_nid_postfix(pointers[127]) == 64);
    Require(sceLibcMspaceMallocUsableSize_nid_postfix(static_cast<unsigned char*>(pointers[127]) + 1) == 0);
    for (std::size_t i = 0; i < pointers.size(); i += 2) {
        sceLibcMspaceFree_nid_postfix(arena, pointers[i]);
        Require(sceLibcMspaceMallocUsableSize_nid_postfix(pointers[i]) == 0);
        sceLibcMspaceFree_nid_postfix(arena, pointers[i]);
    }
    for (std::size_t i = 1; i < pointers.size(); i += 2) {
        for (std::size_t j = 0; j < i % 64 + 1; ++j)
            Require(static_cast<unsigned char*>(pointers[i])[j] == static_cast<unsigned char>(i));
        Require(sceLibcMspaceMallocUsableSize_nid_postfix(pointers[i]) == i % 64 + 1);
        sceLibcMspaceFree_nid_postfix(arena, pointers[i]);
    }
    large = sceLibcMspaceMalloc_nid_postfix(arena, storage.size() - 256);
    Require(large != nullptr);
    Require(sceLibcMspaceDestroy_nid_postfix(arena) == 0);
    Require(sceLibcMspaceMallocUsableSize_nid_postfix(large) == 0);
    Require(sceLibcMspaceDestroy_nid_postfix(otherArena) == 0);
    Require(sceLibcMspaceMalloc_nid_postfix(arena, 8) == nullptr);
    Require(sceLibcMspaceCreate_nid_postfix("reuse", storage.data(), storage.size(), 0) == arena);
    large = sceLibcMspaceMalloc_nid_postfix(arena, storage.size() - 256);
    Require(large != nullptr);
    Require(sceLibcMspaceDestroy_nid_postfix(arena) == 0);
}
