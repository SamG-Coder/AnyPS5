#include <relinker/lowering/NativeFunctions.hpp>
#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

namespace {
using Function = int (__attribute__((sysv_abi)) *)(int);
int __attribute__((sysv_abi)) replacement(int value) { return value + 23; }
void check(bool value) {
    if (!value) throw std::runtime_error("offline native execution regression");
}
class Executable {
public:
    explicit Executable(const std::vector<std::uint8_t>& bytes) : size(bytes.size()) {
#ifdef _WIN32
        address = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!address) throw std::runtime_error("VirtualAlloc failed");
#else
        address = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (address == MAP_FAILED) throw std::runtime_error("mmap failed");
#endif
        std::memcpy(address, bytes.data(), size);
#ifdef _WIN32
        DWORD oldProtection;
        if (!VirtualProtect(address, size, PAGE_EXECUTE_READ, &oldProtection)) {
            VirtualFree(address, 0, MEM_RELEASE);
            throw std::runtime_error("VirtualProtect failed");
        }
        if (!FlushInstructionCache(GetCurrentProcess(), address, size)) {
            VirtualFree(address, 0, MEM_RELEASE);
            throw std::runtime_error("FlushInstructionCache failed");
        }
#else
        if (mprotect(address, size, PROT_READ | PROT_EXEC) != 0) {
            munmap(address, size);
            throw std::runtime_error("mprotect failed");
        }
        __builtin___clear_cache(static_cast<char*>(address), static_cast<char*>(address) + size);
#endif
    }
    ~Executable() {
#ifdef _WIN32
        VirtualFree(address, 0, MEM_RELEASE);
#else
        munmap(address, size);
#endif
    }
    Executable(const Executable&) = delete;
    Executable& operator=(const Executable&) = delete;
    int Run(std::size_t offset, int value) const {
        return reinterpret_cast<Function>(static_cast<std::uint8_t*>(address) + offset)(value);
    }
private:
    void* address;
    std::size_t size;
};
}
int main() {
    std::vector<std::uint8_t> source(256, 0x90);
    const std::array<std::uint8_t, 5> direct{0xe9, 0x3b, 0, 0, 0};
    const std::array<std::uint8_t, 9> indirect{0x48, 0x8d, 0x05, 0x29, 0, 0, 0, 0xff, 0xe0};
    const std::vector<std::uint8_t> body{0xb8, 7, 0, 0, 0, 0xc3};
    std::memcpy(source.data(), direct.data(), direct.size());
    std::memcpy(source.data() + 16, indirect.data(), indirect.size());
    std::memcpy(source.data() + 64, body.data(), body.size());
    {
        Executable original(source);
        check(original.Run(0, 19) == 7 && original.Run(16, 19) == 7);
    }
    Relinker::RelinkResult result{};
    result.OriginalHeaders.push_back({1, 5, 0, 0, 0, source.size(), 4096, 4096});
    result.DynamicSection.DynSymData.resize(24);
    result.DynamicSection.DynStrData.push_back(0);
    const std::array<Relinker::NativeFunctionBinding, 1> bindings{{{64, "native_replacement", "native_fixture.prx", body}}};
    for (const auto opcode : {0xebu, 0x75u, 0xe2u}) {
        auto invalid = source;
        invalid[32] = static_cast<std::uint8_t>(opcode);
        invalid[33] = 33;
        const auto unchanged = invalid;
        auto invalidResult = result;
        bool rejected = false;
        try { Relinker::LowerNativeFunctions(invalid, invalidResult, bindings); }
        catch (const Relinker::RelinkerException&) { rejected = true; }
        check(rejected && invalid == unchanged && invalidResult.OriginalHeaders.size() == 1);
        check(invalidResult.DynamicSection.RelaData.empty());
    }
    const std::array<std::vector<std::uint8_t>, 3> longBranches{{
        {0xe8, 30, 0, 0, 0}, {0xe9, 30, 0, 0, 0}, {0x0f, 0x85, 29, 0, 0, 0}
    }};
    for (const auto& branch : longBranches) {
        auto invalid = source;
        std::copy(branch.begin(), branch.end(), invalid.begin() + 32);
        const auto unchanged = invalid;
        auto invalidResult = result;
        bool rejected = false;
        try { Relinker::LowerNativeFunctions(invalid, invalidResult, bindings); }
        catch (const Relinker::RelinkerException&) { rejected = true; }
        check(rejected && invalid == unchanged && invalidResult.DynamicSection.RelaData.empty());
    }
    Relinker::LowerNativeFunctions(source, result, bindings);
    check(std::equal(direct.begin(), direct.end(), source.begin()));
    check(std::equal(indirect.begin(), indirect.end(), source.begin() + 16));
    check(result.DynamicSection.RelaData.size() == 24);
    std::uint64_t slot, info;
    std::memcpy(&slot, result.DynamicSection.RelaData.data(), 8);
    std::memcpy(&info, result.DynamicSection.RelaData.data() + 8, 8);
    check(slot == 4096 && (info & 0xffffffffu) == 6 && (info >> 32) == 1);
    const auto target = reinterpret_cast<std::uintptr_t>(&replacement);
    std::memcpy(source.data() + slot, &target, sizeof(target));
    Executable lowered(source);
    check(lowered.Run(0, 19) == 42 && lowered.Run(16, 19) == 42);
    check(lowered.Run(64, -23) == 0);
}
