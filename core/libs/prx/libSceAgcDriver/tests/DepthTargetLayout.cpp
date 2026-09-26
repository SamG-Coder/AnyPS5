#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include <array>
#include <cstring>
#include <stdexcept>
#include <vector>

static void Require(bool condition) {
    if (!condition) throw std::runtime_error("depth layout regression");
}

int main() {
    using AgcDriver::Graphics::DepthTargetLayout;
    const DepthTargetLayout tile(128, 128);
    Require(tile.Bytes() == 65536 && tile.Offset(1, 0) == 4 && tile.Offset(0, 1) == 8);
    Require(tile.Offset(16, 0) == 0x2200 && tile.Offset(64, 0) == 0x8400);
    Require(tile.Offset(0, 8) == 0x1100 && tile.Offset(0, 64) == 0x4800);
    Require(tile.Offset(8, 8) == 0x1000);
    std::array<bool, 16384> visited{};
    for (unsigned y = 0; y < 128; ++y) {
        for (unsigned x = 0; x < 128; ++x) {
            const auto offset = tile.Offset(x, y);
            Require(offset < 65536 && offset % 4 == 0 && !visited[offset / 4]);
            visited[offset / 4] = true;
        }
    }
    const DepthTargetLayout layout(129, 131);
    Require(layout.Bytes() == 4 * 65536 && layout.Offset(128, 0) == 65536);
    Require(layout.Offset(0, 128) == 2 * 65536 && layout.Offset(128, 128) == 3 * 65536);
    std::vector<std::byte> linear(layout.LinearBytes()), output(linear.size());
    std::vector<std::byte> tiled(layout.Bytes(), std::byte{0xa5});
    std::vector<bool> touched(layout.Bytes() / 4);
    for (std::size_t i = 0; i < linear.size(); ++i) linear[i] = static_cast<std::byte>((i * 37u + i / 4u) & 255u);
    const std::array<std::uint32_t, 5> bits{0u, 0x80000000u, 0x3f800000u, 0x7fc00001u, 0x7f800000u};
    std::memcpy(linear.data(), bits.data(), sizeof(bits));
    layout.Tile(linear, tiled);
    layout.Detile(tiled, output);
    Require(linear == output);
    for (unsigned y = 0; y < 131; ++y)
        for (unsigned x = 0; x < 129; ++x) touched[layout.Offset(x, y) / 4] = true;
    for (std::size_t i = 0; i < tiled.size(); ++i) if (!touched[i / 4]) Require(tiled[i] == std::byte{0xa5});
    Require(DepthTargetLayout(3840, 2160).Bytes() == 33423360);
    for (const auto extent : {std::array{1u, 1u}, std::array{2u, 5u}, std::array{127u, 129u},
                              std::array{128u, 128u}, std::array{129u, 131u}, std::array{257u, 255u}}) {
        const DepthTargetLayout tested(extent[0], extent[1]);
        std::vector<std::byte> input(tested.LinearBytes()), actual(tested.Bytes(), std::byte{0xa5});
        std::vector<std::byte> expected(tested.Bytes(), std::byte{0xa5}), restored(input.size());
        for (std::size_t i = 0; i < input.size(); ++i) input[i] = static_cast<std::byte>((i * 37u + i / 4u) & 255u);
        for (unsigned y = 0; y < extent[1]; ++y)
            for (unsigned x = 0; x < extent[0]; ++x)
                std::memcpy(expected.data() + tested.Offset(x, y), input.data() + (static_cast<std::size_t>(y) * extent[0] + x) * 4u, 4);
        tested.Tile(input, actual);
        Require(actual == expected);
        tested.Detile(expected, restored);
        Require(restored == input);
    }
    unsigned rejected = 0;
    try { (void)DepthTargetLayout(0, 1); } catch (const std::runtime_error&) { ++rejected; }
    try { (void)DepthTargetLayout(16385, 1); } catch (const std::runtime_error&) { ++rejected; }
    try { (void)layout.Offset(129, 0); } catch (const std::runtime_error&) { ++rejected; }
    try { layout.Tile(std::span(linear).subspan(1), tiled); } catch (const std::runtime_error&) { ++rejected; }
    try { layout.Detile(std::span(tiled).subspan(1), output); } catch (const std::runtime_error&) { ++rejected; }
    Require(rejected == 5);
}
