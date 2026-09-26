#include "prx/libSceAgcDriver/Graphics/include/DepthStateDecode.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DepthState.hpp"
#include <array>
#include <stdexcept>
#include <string>
#include <tuple>

static void Require(bool condition) {
    if (!condition) throw std::runtime_error("depth state regression");
}

int main() {
    using namespace AgcDriver;
    using Graphics::DecodeDepthState;
    Registers registers{{0x200, 0x76}, {0, 0}, {2, 0}, {7, 0x086f0eff},
        {0x10, 0x80000183}, {0x12, 0x22a000}, {0x14, 0x22a000}, {0x1a, 0}, {0x1c, 0}};
    const auto depth = DecodeDepthState(registers);
    Require(depth && depth->address == 0x22a00000 && depth->extent.width == 3840 &&
        depth->extent.height == 2160 && depth->bytes == 33423360 &&
        depth->compare == VK_COMPARE_OP_ALWAYS && depth->writeEnabled);
    const std::array comparisons{VK_COMPARE_OP_NEVER, VK_COMPARE_OP_LESS, VK_COMPARE_OP_EQUAL,
        VK_COMPARE_OP_LESS_OR_EQUAL, VK_COMPARE_OP_GREATER, VK_COMPARE_OP_NOT_EQUAL,
        VK_COMPARE_OP_GREATER_OR_EQUAL, VK_COMPARE_OP_ALWAYS};
    for (unsigned i = 0; i < comparisons.size(); ++i) {
        auto copy = registers;
        copy[0x200] = (i << 4u) | 2u;
        copy.erase(0x14);
        copy.erase(0x1c);
        const auto result = DecodeDepthState(copy);
        Require(result && result->compare == comparisons[i] && !result->writeEnabled);
    }
    Require(!DecodeDepthState(Registers{{0x200, 0}}));
    Require(!DecodeDepthState(Registers{{0x200, 0x007007f4}}));
    auto extended = registers;
    extended[0x1a] = extended[0x1c] = 0xab;
    Require(DecodeDepthState(extended)->address == 0xab0022a00000ull);
    extended[7] = 0;
    Require(DecodeDepthState(extended)->bytes == 65536);
    extended[7] = 0x3fff3fff;
    Require(DecodeDepthState(extended)->bytes == 1073741824ull);
    const auto reject = [&](Registers invalid, const char* reason) {
        bool rejected = false;
        try { (void)DecodeDepthState(invalid); }
        catch (const std::runtime_error& error) { rejected = std::string(error.what()).find(reason) != std::string::npos; }
        Require(rejected);
    };
    for (auto bit : {1u, 8u, 0x40000000u, 0x80000000u}) {
        auto invalid = registers;
        invalid[0x200] |= bit;
        reject(invalid, "stencil, depth bounds or conditional");
    }
    for (const auto& [reg, value, reason] : std::array{
        std::tuple{0u, 1u, "special render controls"},
        std::tuple{2u, 0x04000000u, "depth mip"},
        std::tuple{0x10u, 0x8000018bu, "single-sample D32"},
        std::tuple{0x10u, 0x80000181u, "single-sample D32"},
        std::tuple{0x10u, 0xa0000183u, "single-sample D32"},
        std::tuple{7u, 0x80000000u, "reserved depth extent"},
        std::tuple{0x1au, 0x100u, "address extension"},
        std::tuple{0x12u, 0u, "64KB-aligned"},
        std::tuple{0x12u, 0x22a001u, "64KB-aligned"},
        std::tuple{0x14u, 0x22b000u, "separate depth"}}) {
        auto invalid = registers;
        invalid[reg] = value;
        reject(invalid, reason);
    }
    for (const auto& [reg, value] : registers) {
        auto invalid = registers;
        invalid.erase(reg);
        reject(invalid, "missing depth register");
    }
}
