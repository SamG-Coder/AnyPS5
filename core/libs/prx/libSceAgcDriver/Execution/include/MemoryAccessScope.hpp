#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_MEMORYACCESSSCOPE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_MEMORYACCESSSCOPE_HPP

#include <cstddef>
#include <cstdint>

namespace AgcDriver::GuestMemory {

class MemoryAccessScope {
public:
    using Resolver = void (*)(void*, std::uint64_t, std::size_t, bool);
    MemoryAccessScope(void* context, Resolver resolver);
    ~MemoryAccessScope();
    MemoryAccessScope(const MemoryAccessScope&) = delete;
    MemoryAccessScope& operator=(const MemoryAccessScope&) = delete;
    static void Resolve(std::uint64_t address, std::size_t bytes, bool writable);

private:
    static thread_local void* currentContext;
    static thread_local Resolver currentResolver;
    void* previousContext;
    Resolver previousResolver;
};

}

#endif
