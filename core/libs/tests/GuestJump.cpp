#include "prx/libc/include/general/VabiMacros.hpp"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cfenv>

extern "C" {
__attribute__((returns_twice)) int APS5_VABI setjmp_nid_postfix(void*);
[[noreturn]] void APS5_VABI longjmp_nid_postfix(void*, int);
}
static void Require(bool value) { if (!value) std::abort(); }
static void APS5_VABI Jump(void* buffer, int value) {
    volatile std::uint64_t scratch[128]{};
    for (unsigned i = 0; i < 128; ++i) scratch[i] = i;
    longjmp_nid_postfix(buffer, value);
}
static void APS5_VABI Check(int value) {
    struct Guarded {
        std::uint64_t before = UINT64_MAX;
        alignas(8) std::array<std::uint64_t, 12> buffer{};
        std::array<std::uint64_t, 32> after;
    } saved;
    saved.after.fill(UINT64_MAX);
    Require(std::fesetround(FE_DOWNWARD) == 0);
    volatile int visits = 0;
    const int result = setjmp_nid_postfix(saved.buffer.data());
    if (visits == 0) {
        Require(result == 0);
        Require(saved.before == UINT64_MAX);
        for (auto guard : saved.after) Require(guard == UINT64_MAX);
        visits = 1;
        Require(std::fesetround(FE_UPWARD) == 0);
        Jump(saved.buffer.data(), value);
    }
    Require(visits == 1 && result == (value == 0 ? 1 : value));
    Require(std::fegetround() == FE_DOWNWARD);
    Require(saved.before == UINT64_MAX);
    for (auto guard : saved.after) Require(guard == UINT64_MAX);
}
int main() {
    const int rounding = std::fegetround();
    Check(0);
    Check(17);
    Check(-3);
    Require(std::fesetround(rounding) == 0);
}
