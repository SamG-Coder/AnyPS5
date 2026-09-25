#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <mutex>
#include <stdexcept>

namespace {
std::mutex randomMutex;
std::uint32_t randomState = 1;

int NextRandom(std::uint32_t& state) {
    const std::uint64_t normalized = state % UINT32_C(2147483646) + UINT64_C(1);
    state = static_cast<std::uint32_t>((normalized * 16807) % 2147483647 - 1);
    return static_cast<int>(state);
}
}

extern "C" int APS5_VABI rand_nid_postfix() {
    std::lock_guard lock(randomMutex);
    return NextRandom(randomState);
}

extern "C" void APS5_VABI srand_nid_postfix(std::uint32_t seed) {
    std::lock_guard lock(randomMutex);
    randomState = seed;
}

extern "C" int APS5_VABI rand_r_nid_postfix(std::uint32_t* seed) {
    if (!seed) throw std::invalid_argument("rand_r: null seed");
    return NextRandom(*seed);
}
