#include "DirectMemory.hpp"
#include "prx/libc/include/GuestAllocations.hpp"
#include <cerrno>
#include <limits>
#include <stdexcept>
#include <system_error>

#if defined(__linux__)
#include <sys/mman.h>
#else
#include <windows.h>

static constexpr int PROT_NONE = 0;
static constexpr int PROT_READ = 1;
static constexpr int PROT_WRITE = 2;
static constexpr int PROT_EXEC = 4;
static constexpr int MAP_PRIVATE = 0x02;
static constexpr int MAP_ANONYMOUS = 0x20;
static constexpr int MAP_FIXED = 0x10;
static void* const MAP_FAILED = reinterpret_cast<void*>(-1);

static DWORD WinProtFromPosix(int prot) {
    if (prot == PROT_NONE) return PAGE_NOACCESS;
    if ((prot & PROT_EXEC) && (prot & PROT_WRITE)) return PAGE_EXECUTE_READWRITE;
    if ((prot & PROT_EXEC) && (prot & PROT_READ)) return PAGE_EXECUTE_READ;
    if (prot & PROT_EXEC) return PAGE_EXECUTE;
    if (prot & PROT_WRITE) return PAGE_READWRITE;
    return PAGE_READONLY;
}

static void* mmap(void* addr, size_t len, int prot, int flags, int, int) {
    DWORD winProt = WinProtFromPosix(prot);
    if (flags & MAP_FIXED) {
        void* result = VirtualAlloc(addr, len, MEM_RESERVE | MEM_COMMIT, winProt);
        if (!result) throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "VirtualAlloc fixed failed");
        return result;
    }
    void* result = VirtualAlloc(nullptr, len, MEM_RESERVE | MEM_COMMIT, winProt);
    if (!result) throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "VirtualAlloc failed");
    return result;
}

static int munmap(void* addr, size_t len) {
    if (!VirtualFree(addr, len, MEM_DECOMMIT))
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "VirtualFree decommit failed");
    return 0;
}

static int munmap_release(void* addr) {
    if (!VirtualFree(addr, 0, MEM_RELEASE))
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "VirtualFree release failed");
    return 0;
}

static int mprotect(void* addr, size_t len, int prot) {
    DWORD old;
    if (!VirtualProtect(addr, len, WinProtFromPosix(prot), &old))
        throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), "VirtualProtect failed");
    return 0;
}
#endif

namespace {

void ValidateLength(size_t len) {
    if (len == 0 || (len & (PS5_PAGE_SIZE - 1)) != 0) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Memory length must be a positive multiple of the guest page size");
    }
}

size_t ValidateAlignment(size_t alignment) {
    if (alignment == 0) return PS5_PAGE_SIZE;
    if (alignment < PS5_PAGE_SIZE || (alignment & (alignment - 1)) != 0) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Memory alignment must be a power of two no smaller than the guest page size");
    }
    return alignment;
}

void ValidateRange(const void* addr, size_t len, size_t alignment) {
    ValidateLength(len);
    const auto start = reinterpret_cast<std::uintptr_t>(addr);
    if (!addr || (start & (alignment - 1)) != 0 || len > std::numeric_limits<std::uintptr_t>::max() - start) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Invalid memory address, alignment or range");
    }
}

int LinuxProtFromSce(int prot) {
    if ((prot & ~0x37) != 0) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Unsupported memory protection bits");
    }
    int result = PROT_NONE;
    if (prot & 1) result |= PROT_READ;
    if (prot & 2) result |= PROT_WRITE;
    if (prot & 4) result |= PROT_EXEC;
    return result;
}

void Unmap(void* addr, size_t len) {
#if defined(__linux__)
    if (munmap(addr, len) != 0) throw std::system_error(errno, std::generic_category(), "munmap failed");
#else
    munmap_release(addr);
#endif
}

void* MapAligned(void* addr, size_t len, int prot, int flags, size_t alignment) {
    ValidateLength(len);
    alignment = ValidateAlignment(alignment);
    constexpr int GuestMapFixed = 0x10;
    constexpr int GuestMapNoCoalesce = 0x400000;
    if ((flags & ~(GuestMapFixed | GuestMapNoCoalesce)) != 0) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Unsupported memory mapping flags");
    }
    if ((flags & GuestMapFixed) != 0) {
        ValidateRange(addr, len, alignment);
        void* result = mmap(addr, len, prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (result == MAP_FAILED) {
            // return SCE_KERNEL_ERROR_ENOMEM;
            throw std::system_error(errno, std::generic_category(), "Fixed mmap failed");
        }
        return result;
    }
    if (addr) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Non-fixed mapping address hints are not implemented");
    }
    if (len > std::numeric_limits<size_t>::max() - alignment) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::overflow_error("Aligned mapping size overflow");
    }
    const size_t allocLen = len + alignment;
    void* result = mmap(nullptr, allocLen, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
        // return SCE_KERNEL_ERROR_ENOMEM;
        throw std::system_error(errno, std::generic_category(), "Aligned mmap failed");
    }
    const auto raw = reinterpret_cast<std::uintptr_t>(result);
    const size_t prefix = (alignment - (raw & (alignment - 1))) & (alignment - 1);
    void* aligned = reinterpret_cast<void*>(raw + prefix);
    const size_t suffix = allocLen - prefix - len;
    if (prefix != 0 && munmap(result, prefix) != 0) {
        const int error = errno;
        Unmap(result, allocLen);
        throw std::system_error(error, std::generic_category(), "Mapping prefix munmap failed");
    }
    if (suffix != 0 && munmap(reinterpret_cast<void*>(raw + prefix + len), suffix) != 0) {
        const int error = errno;
        Unmap(aligned, allocLen - prefix);
        throw std::system_error(error, std::generic_category(), "Mapping suffix munmap failed");
    }
    return aligned;
}

void ValidateOutput(void** addr) {
    if (!addr) {
        // return SCE_KERNEL_ERROR_EINVAL;
        throw std::invalid_argument("Null memory mapping output");
    }
}

}

int DoMapDirect(void** addr, size_t len, int prot, int flags, int64_t physStart, size_t alignment) {
    ValidateOutput(addr);
    if (len == 0 || (len & (PS5_PAGE_SIZE - 1)) != 0) return SCE_KERNEL_ERROR_EINVAL;
    if (physStart < 0 || (static_cast<std::uint64_t>(physStart) & (PS5_PAGE_SIZE - 1)) != 0 || static_cast<std::uint64_t>(physStart) >= DIRECT_MEMORY_SIZE || len > DIRECT_MEMORY_SIZE - static_cast<std::uint64_t>(physStart)) {
        return SCE_KERNEL_ERROR_EINVAL;
    }
    GuestAllocations::Mutation mutation;
    if (*addr != nullptr) mutation.RequireAvailable(*addr, len);
    void* mapped = MapAligned(*addr, len, LinuxProtFromSce(prot), flags, alignment);
    try {
        mutation.Add(mapped, len, (prot & 3) != 0, (prot & 2) != 0);
    } catch (...) {
        Unmap(mapped, len);
        throw;
    }
    *addr = mapped;
    return 0;
}

int DoMapAnon(void** addr, size_t len, int prot, int flags) {
    ValidateOutput(addr);
    if (len == 0 || (len & (PS5_PAGE_SIZE - 1)) != 0) return SCE_KERNEL_ERROR_EINVAL;
    GuestAllocations::Mutation mutation;
    if (*addr != nullptr) mutation.RequireAvailable(*addr, len);
    void* mapped = MapAligned(*addr, len, LinuxProtFromSce(prot), flags, PS5_PAGE_SIZE);
    try {
        mutation.Add(mapped, len, (prot & 3) != 0, (prot & 2) != 0);
    } catch (...) {
        Unmap(mapped, len);
        throw;
    }
    *addr = mapped;
    return 0;
}

int DoMprotect(const void* addr, size_t len, int prot) {
    const auto address = reinterpret_cast<std::uintptr_t>(addr);
    constexpr auto pageMask = static_cast<std::uintptr_t>(PS5_PAGE_SIZE - 1);
    const auto limit = std::numeric_limits<std::uintptr_t>::max();
    if (address == 0 || len == 0 || len > limit - address || address + len > limit - pageMask) throw std::invalid_argument("Invalid guest memory protection range");
    const auto first = address & ~pageMask;
    const auto end = (address + len + pageMask) & ~pageMask;
    const auto bytes = static_cast<std::size_t>(end - first);
    const auto* pointer = reinterpret_cast<const void*>(first);
    const auto nativeProtection = LinuxProtFromSce(prot);
    GuestAllocations::Mutation mutation;
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION memory{};
    if (VirtualQuery(pointer, &memory, sizeof(memory)) != sizeof(memory)) throw std::runtime_error("Cannot query guest memory protection range");
    if (memory.Type == MEM_IMAGE) {
        if (memory.AllocationBase != GetModuleHandleW(nullptr)) throw std::invalid_argument("Memory protection of a foreign image is not supported");
        mutation.RegisterMainImage();
    }
#endif
    mutation.Protect(pointer, bytes, (prot & 3) != 0, (prot & 2) != 0, [&] {
        if (mprotect(const_cast<void*>(pointer), bytes, nativeProtection) != 0) throw std::system_error(errno, std::generic_category(), "mprotect failed");
    });
    return 0;
}

int DoMunmap(void* addr, size_t len) {
    if (len == 0 || (len & (PS5_PAGE_SIZE - 1)) != 0 || !addr) return SCE_KERNEL_ERROR_EINVAL;
    GuestAllocations::Mutation mutation;
    mutation.Unmap(addr, len, [&](const void* allocation, bool last) {
#if defined(__linux__)
        Unmap(addr, len);
#else
        if (last) munmap_release(const_cast<void*>(allocation));
        else munmap(addr, len);
#endif
    });
    return 0;
}

int DoReserveVirtual(void** addr, size_t len, size_t alignment) {
    ValidateOutput(addr);
    if (len == 0 || (len & (PS5_PAGE_SIZE - 1)) != 0) return SCE_KERNEL_ERROR_EINVAL;
    GuestAllocations::Mutation mutation;
    void* mapped = MapAligned(nullptr, len, PROT_NONE, 0, alignment);
    try {
        mutation.Add(mapped, len, false, false);
    } catch (...) {
        Unmap(mapped, len);
        throw;
    }
    *addr = mapped;
    return 0;
}
