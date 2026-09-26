#include "prx/libc/include/MemoryBackingPlatform.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <system_error>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace GuestMemoryBacking::Platform {
namespace {

DWORD nativeProtection(int protection) {
    if ((protection & 4) != 0) return (protection & 2) != 0 ? PAGE_EXECUTE_READWRITE : (protection & 1) != 0 ? PAGE_EXECUTE_READ : PAGE_EXECUTE;
    if ((protection & 2) != 0) return PAGE_READWRITE;
    return (protection & 1) != 0 ? PAGE_READONLY : PAGE_NOACCESS;
}

void check(bool success, const char* operation) {
    if (!success) throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), operation);
}

class LowAddressArena {
    using AllocateFunction = PVOID (WINAPI*)(HANDLE, PVOID, SIZE_T, ULONG, ULONG, MEM_EXTENDED_PARAMETER*, ULONG);
    using MapFunction = PVOID (WINAPI*)(HANDLE, HANDLE, PVOID, ULONG64, SIZE_T, ULONG, ULONG, MEM_EXTENDED_PARAMETER*, ULONG);
public:
    LowAddressArena() {
        const auto kernel = GetModuleHandleW(L"kernelbase.dll");
        const auto allocate = reinterpret_cast<AllocateFunction>(GetProcAddress(kernel, "VirtualAlloc2"));
        map = reinterpret_cast<MapFunction>(GetProcAddress(kernel, "MapViewOfFile3"));
        unmap = reinterpret_cast<decltype(&UnmapViewOfFile2)>(GetProcAddress(kernel, "UnmapViewOfFile2"));
        if (allocate == nullptr || map == nullptr || unmap == nullptr) return;
        MEM_ADDRESS_REQUIREMENTS requirements{};
        requirements.LowestStartingAddress = reinterpret_cast<void*>(0x10000);
        requirements.HighestEndingAddress = reinterpret_cast<void*>(0xffffffffull);
        requirements.Alignment = 0x10000;
        MEM_EXTENDED_PARAMETER parameter{};
        parameter.Type = MemExtendedParameterAddressRequirements;
        parameter.Pointer = &requirements;
        for (std::size_t size = 0x80000000ull; size >= 0x10000000ull; size /= 2) {
            void* reservation = allocate(GetCurrentProcess(), nullptr, size, MEM_RESERVE | MEM_RESERVE_PLACEHOLDER,
                                         PAGE_NOACCESS, &parameter, 1);
            if (reservation != nullptr) {
                begin = reinterpret_cast<std::uintptr_t>(reservation);
                end = begin + size;
                break;
            }
        }
    }

    bool Contains(std::uintptr_t address) const { return begin != 0 && address >= begin && address < end; }

    void* Map(HANDLE section, void* requested, std::size_t bytes, std::size_t alignment) {
        const auto wanted = reinterpret_cast<std::uintptr_t>(requested);
        if (requested != nullptr && !Contains(wanted)) return nullptr;
        Coalesce();
        for (auto cursor = begin; cursor < end;) {
            MEMORY_BASIC_INFORMATION info{};
            check(VirtualQuery(reinterpret_cast<void*>(cursor), &info, sizeof(info)) != 0, "VirtualQuery guest arena");
            const auto next = cursor + info.RegionSize;
            const auto candidate = requested != nullptr ? wanted : (cursor + alignment - 1) & ~(alignment - 1);
            if (info.State == MEM_RESERVE && candidate >= cursor && candidate < next && bytes <= next - candidate) {
                if (candidate != cursor) check(VirtualFree(reinterpret_cast<void*>(cursor), candidate - cursor,
                    MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER) != FALSE, "split guest arena prefix");
                if (bytes < next - candidate) check(VirtualFree(reinterpret_cast<void*>(candidate), bytes,
                    MEM_RELEASE | MEM_PRESERVE_PLACEHOLDER) != FALSE, "split guest arena suffix");
                void* result = map(section, GetCurrentProcess(), reinterpret_cast<void*>(candidate), 0, bytes,
                                   MEM_REPLACE_PLACEHOLDER, PAGE_EXECUTE_READWRITE, nullptr, 0);
                check(result != nullptr, "MapViewOfFile3 guest arena");
                return result;
            }
            cursor = next;
        }
        if (requested != nullptr) throw std::runtime_error("fixed guest mapping overlaps occupied arena memory");
        return nullptr;
    }

    void Unmap(void* address) {
        check(unmap(GetCurrentProcess(), address, MEM_PRESERVE_PLACEHOLDER) != FALSE, "UnmapViewOfFile2 guest arena");
    }

private:
    void Coalesce() {
        std::uintptr_t freeBegin = 0;
        for (auto cursor = begin; cursor < end;) {
            MEMORY_BASIC_INFORMATION info{};
            check(VirtualQuery(reinterpret_cast<void*>(cursor), &info, sizeof(info)) != 0, "VirtualQuery guest arena coalesce");
            const auto next = cursor + info.RegionSize;
            if (info.State != MEM_RESERVE) freeBegin = 0;
            else if (freeBegin == 0) freeBegin = cursor;
            else check(VirtualFree(reinterpret_cast<void*>(freeBegin), next - freeBegin,
                       MEM_RELEASE | MEM_COALESCE_PLACEHOLDERS) != FALSE, "coalesce guest arena placeholders");
            cursor = next;
        }
    }

    std::uintptr_t begin = 0;
    std::uintptr_t end = 0;
    MapFunction map = nullptr;
    decltype(&UnmapViewOfFile2) unmap = nullptr;
};

LowAddressArena& arena() {
    static LowAddressArena value;
    return value;
}

void unmapGuest(void* address) {
    if (arena().Contains(reinterpret_cast<std::uintptr_t>(address))) arena().Unmap(address);
    else check(UnmapViewOfFile(address) != FALSE, "UnmapViewOfFile guest view");
}

}

Mapping Map(void* address, std::size_t bytes, std::size_t alignment, int protection) {
    SYSTEM_INFO info{};
    GetSystemInfo(&info);
    alignment = std::max(alignment, static_cast<std::size_t>(info.dwAllocationGranularity));
    if (address != nullptr && reinterpret_cast<std::uintptr_t>(address) % info.dwAllocationGranularity != 0) throw std::invalid_argument("fixed guest view is not aligned to native allocation granularity");
    auto& lowArena = arena();
    const auto size = static_cast<std::uint64_t>(bytes);
    HANDLE section = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_EXECUTE_READWRITE, static_cast<DWORD>(size >> 32u), static_cast<DWORD>(size), nullptr);
    check(section != nullptr, "CreateFileMapping guest backing");
    void* alias = nullptr;
    void* guest = nullptr;
    try {
        alias = MapViewOfFile(section, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, bytes);
        check(alias != nullptr, "MapViewOfFile guest backing alias");
        guest = lowArena.Map(section, address, bytes, alignment);
        if (guest == nullptr && address == nullptr && alignment > info.dwAllocationGranularity) {
            if (bytes > std::numeric_limits<std::size_t>::max() - alignment) throw std::overflow_error("aligned guest backing reservation overflow");
            void* reservation = VirtualAlloc(nullptr, bytes + alignment, MEM_RESERVE, PAGE_NOACCESS);
            check(reservation != nullptr, "VirtualAlloc guest backing reservation");
            const auto first = reinterpret_cast<std::uintptr_t>(reservation);
            address = reinterpret_cast<void*>((first + alignment - 1) & ~(static_cast<std::uintptr_t>(alignment) - 1));
            check(VirtualFree(reservation, 0, MEM_RELEASE) != FALSE, "VirtualFree guest backing reservation");
        }
        if (guest == nullptr) guest = MapViewOfFileEx(section, FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE, 0, 0, bytes, address);
        check(guest != nullptr, "MapViewOfFileEx guest memory");
        if (reinterpret_cast<std::uintptr_t>(guest) % alignment != 0 || (address != nullptr && guest != address)) throw std::runtime_error("guest backing view address mismatch");
        DWORD previous = 0;
        check(VirtualProtect(guest, bytes, nativeProtection(protection), &previous) != FALSE, "VirtualProtect guest backing view");
        return {reinterpret_cast<std::uintptr_t>(guest), bytes, alias, reinterpret_cast<std::uintptr_t>(section)};
    } catch (...) {
        if (guest != nullptr) unmapGuest(guest);
        if (alias != nullptr) check(UnmapViewOfFile(alias) != FALSE, "UnmapViewOfFile failed guest alias");
        check(CloseHandle(section) != FALSE, "CloseHandle failed guest backing");
        throw;
    }
}

void Unmap(const Mapping& mapping) {
    unmapGuest(reinterpret_cast<void*>(mapping.address));
    check(UnmapViewOfFile(mapping.alias) != FALSE, "UnmapViewOfFile guest alias");
    check(CloseHandle(reinterpret_cast<HANDLE>(mapping.handle)) != FALSE, "CloseHandle guest backing");
}

void Deactivate(std::uint64_t address, std::size_t bytes) {
    DWORD previous = 0;
    check(VirtualProtect(reinterpret_cast<void*>(address), bytes, PAGE_NOACCESS, &previous) != FALSE, "VirtualProtect guest backing unmap");
}

}
