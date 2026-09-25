#include "../include/Pthread.hpp"
#include <array>
#include <mutex>

namespace {
constexpr int KeyLimit = 256;
constexpr int DestructorIterations = 4;
constexpr int ErrorInvalid = 22;
constexpr int ErrorAgain = 35;

struct KeyDefinition {
    bool live = false;
    std::uint64_t generation = 0;
    void (APS5_VABI *destructor)(void*) = nullptr;
};

struct ThreadSlot {
    std::uint64_t generation = 0;
    void* value = nullptr;
};

struct ThreadStorage {
    std::array<ThreadSlot, KeyLimit> slots{};
    bool running = false;
    ~ThreadStorage();
};

struct Table {
    std::mutex mutex;
    std::array<KeyDefinition, KeyLimit> keys{};
};

Table& Keys() {
    static auto* table = new Table;
    return *table;
}

thread_local ThreadStorage storage;

ThreadStorage& Current() { return storage; }

void Destroy(ThreadStorage& thread) {
    if (thread.running) return;
    thread.running = true;
    for (int iteration = 0; iteration < DestructorIterations; ++iteration) {
        struct Call { void (APS5_VABI *destructor)(void*) = nullptr; void* value = nullptr; };
        std::array<Call, KeyLimit> calls{};
        bool pending = false;
        {
            std::lock_guard lock(Keys().mutex);
            for (int index = 0; index < KeyLimit; ++index) {
                auto& defined = Keys().keys[static_cast<std::size_t>(index)];
                auto& slot = thread.slots[static_cast<std::size_t>(index)];
                if (!defined.live || !defined.destructor || slot.generation != defined.generation || !slot.value)
                    continue;
                calls[static_cast<std::size_t>(index)] = {defined.destructor, slot.value};
                slot.value = nullptr;
                pending = true;
            }
        }
        if (!pending) break;
        for (const auto& call : calls)
            if (call.destructor) call.destructor(call.value);
    }
    for (auto& slot : thread.slots) slot.value = nullptr;
    thread.running = false;
}

int KernelError(int error) { return static_cast<int>(0x80020000u | static_cast<unsigned>(error)); }

ThreadStorage::~ThreadStorage() { Destroy(*this); }
}

extern "C" void RunGuestThreadKeys_nid_no_patch() { Destroy(Current()); }

extern "C" {

int APS5_VABI scePthreadKeyCreate(PthreadKey* key, pthread_key_destructor_func_t destructor) {
    if (!key) return KernelError(ErrorInvalid);
    std::lock_guard lock(Keys().mutex);
    for (int index = 0; index < KeyLimit; ++index) {
        auto& defined = Keys().keys[static_cast<std::size_t>(index)];
        if (defined.live) continue;
        ++defined.generation;
        defined.destructor = reinterpret_cast<void (APS5_VABI *)(void*)>(destructor);
        defined.live = true;
        *key = index;
        return 0;
    }
    return KernelError(ErrorAgain);
}

int APS5_VABI scePthreadKeyDelete(PthreadKey key) {
    if (key < 0 || key >= KeyLimit) return KernelError(ErrorInvalid);
    std::lock_guard lock(Keys().mutex);
    auto& defined = Keys().keys[static_cast<std::size_t>(key)];
    if (!defined.live) return KernelError(ErrorInvalid);
    defined.live = false;
    defined.destructor = nullptr;
    return 0;
}

void* APS5_VABI scePthreadGetspecific(PthreadKey key) {
    if (key < 0 || key >= KeyLimit) return nullptr;
    auto& local = Current();
    std::lock_guard lock(Keys().mutex);
    auto& defined = Keys().keys[static_cast<std::size_t>(key)];
    if (!defined.live) return nullptr;
    auto& slot = local.slots[static_cast<std::size_t>(key)];
    if (slot.generation != defined.generation) return nullptr;
    return slot.value;
}

int APS5_VABI scePthreadSetspecific(PthreadKey key, void* value) {
    if (key < 0 || key >= KeyLimit) return KernelError(ErrorInvalid);
    auto& local = Current();
    std::lock_guard lock(Keys().mutex);
    auto& defined = Keys().keys[static_cast<std::size_t>(key)];
    if (!defined.live) return KernelError(ErrorInvalid);
    auto& slot = local.slots[static_cast<std::size_t>(key)];
    slot.generation = defined.generation;
    slot.value = value;
    return 0;
}

}
