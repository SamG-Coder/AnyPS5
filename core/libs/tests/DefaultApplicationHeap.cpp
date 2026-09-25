#include "prx/libc/include/ApplicationHeap.hpp"
#include "prx/libc/include/GuestHeap.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <thread>

static void Require(bool condition) { if (!condition) std::abort(); }
template<typename TAction> static void Reject(TAction action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    Require(rejected);
}
template<typename TValue, std::size_t TSize>
static void Write(std::array<std::byte, TSize>& bytes, std::size_t offset, TValue value) {
    std::memcpy(bytes.data() + offset, &value, sizeof(value));
}
static void Exercise() {
    auto* pointer = static_cast<unsigned char*>(ApplicationHeapAllocate_nid_no_patch(37));
    Require(pointer && reinterpret_cast<std::uintptr_t>(pointer) % alignof(std::max_align_t) == 0);
    Require(ApplicationHeapUsableSize_nid_no_patch(pointer) == 37);
    std::memset(pointer, 0x5a, 37);
    pointer = static_cast<unsigned char*>(ApplicationHeapReallocate_nid_no_patch(pointer, 150));
    for (unsigned index = 0; index < 37; ++index) Require(pointer[index] == 0x5a);
    Require(ApplicationHeapUsableSize_nid_no_patch(pointer) == 150);
    pointer = static_cast<unsigned char*>(ApplicationHeapReallocate_nid_no_patch(pointer, 7));
    for (unsigned index = 0; index < 7; ++index) Require(pointer[index] == 0x5a);
    Require(ApplicationHeapReallocate_nid_no_patch(pointer, 0) == nullptr);
    pointer = static_cast<unsigned char*>(ApplicationHeapCalloc_nid_no_patch(7, 9));
    for (unsigned index = 0; index < 63; ++index) Require(pointer[index] == 0);
    ApplicationHeapFree_nid_no_patch(pointer);
    for (const std::size_t alignment : {16, 64, 4096}) {
        auto* aligned = ApplicationHeapAlign_nid_no_patch(alignment, 33);
        Require(reinterpret_cast<std::uintptr_t>(aligned) % alignment == 0);
        Require(ApplicationHeapUsableSize_nid_no_patch(aligned) == 33);
        ApplicationHeapFree_nid_no_patch(aligned);
    }
    void* aligned = nullptr;
    Require(ApplicationHeapPosixAlign_nid_no_patch(&aligned, 256, 99) == 0);
    Require(reinterpret_cast<std::uintptr_t>(aligned) % 256 == 0);
    ApplicationHeapFree_nid_no_patch(aligned);
}
int main() {
    std::array<void*, 10> partial{};
    partial[0] = reinterpret_cast<void*>(ApplicationHeapAllocate_nid_no_patch);
    Reject([&] { ApplicationHeapRegister_nid_no_patch(partial.data()); });
    std::array<std::byte, 0x40> process{};
    std::array<std::byte, 0x38> libc{};
    std::array<std::byte, 0x78> replacement{};
    Write(process, 0, std::uint64_t{0x40});
    Write(process, 8, std::uint32_t{0x4942524f});
    Write(process, 0x38, libc.data());
    Write(libc, 0, std::uint64_t{0x38});
    Write(libc, 0x30, replacement.data());
    Write(replacement, 0, std::uint64_t{0x78});
    Write(replacement, 8, std::uint64_t{2});
    ApplicationHeapInitialize_nid_no_patch(process.data());
    ApplicationHeapInitialize_nid_no_patch(process.data());
    Exercise();
    std::array<std::thread, 4> workers;
    for (auto& worker : workers) worker = std::thread([] { for (unsigned index = 0; index < 20; ++index) Exercise(); });
    for (auto& worker : workers) worker.join();
    auto* zero = ApplicationHeapAllocate_nid_no_patch(0);
    Require(zero != nullptr);
    ApplicationHeapFree_nid_no_patch(zero);
    Require(ApplicationHeapUsableSize_nid_no_patch(nullptr) == 0);
    ApplicationHeapFree_nid_no_patch(nullptr);
    int foreign = 0;
    Reject([&] { ApplicationHeapUsableSize_nid_no_patch(&foreign); });
    Reject([&] { ApplicationHeapFree_nid_no_patch(&foreign); });
    Reject([] { ApplicationHeapCalloc_nid_no_patch(SIZE_MAX, 2); });
    auto* original = static_cast<unsigned char*>(ApplicationHeapAllocate_nid_no_patch(8));
    original[0] = 42;
    Reject([&] { ApplicationHeapReallocate_nid_no_patch(original, SIZE_MAX); });
    Require(original[0] == 42 && ApplicationHeapUsableSize_nid_no_patch(original) == 8);
    ApplicationHeapFree_nid_no_patch(original);
}
