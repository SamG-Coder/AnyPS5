#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>

namespace AgcDriver::Graphics {
namespace {

constexpr std::uint32_t localOffset(std::uint32_t x, std::uint32_t y) {
    const auto bit = [](std::uint32_t value, unsigned source, unsigned destination) {
        return ((value >> source) & 1u) << destination;
    };
    return bit(x, 0, 2) | bit(y, 0, 3) | bit(x, 1, 4) | bit(y, 1, 5) |
        bit(x, 2, 6) | bit(y, 2, 7) | bit(x ^ y, 3, 8) | bit(x ^ y, 4, 9) |
        (((y >> 5u ^ x >> 6u) & 1u) << 10u) | (((x >> 5u ^ y >> 6u) & 1u) << 11u) |
        bit(y, 3, 12) | bit(x, 4, 13) | bit(y, 6, 14) | bit(x, 6, 15);
}

constexpr auto xOffsets = [] {
    std::array<std::uint32_t, 128> result{};
    for (unsigned x = 0; x < result.size(); ++x) result[x] = localOffset(x, 0);
    return result;
}();

template<bool toTiled>
void transfer(const std::byte* source, std::byte* destination, std::uint32_t width,
              std::uint32_t height, std::uint32_t blocksPerRow) {
    const auto* offsets = xOffsets.data();
    for (std::uint32_t y = 0; y < height; ++y) {
        const auto row = static_cast<std::size_t>(y) * width * 4u;
        const auto tileRow = static_cast<std::size_t>(y / 128u) * blocksPerRow * 65536u;
        const auto yOffset = localOffset(0, y & 127u);
        for (std::uint32_t block = 0; block < blocksPerRow; ++block) {
            const auto count = std::min(128u, width - block * 128u);
            const auto linearBase = row + block * 512u;
            const auto tiledBase = tileRow + block * 65536u;
            std::uint32_t x = 0;
            for (; x + 1 < count; x += 2) {
                const auto linearOffset = linearBase + x * 4u;
                const auto tiledOffset = tiledBase + (offsets[x] ^ yOffset);
                if constexpr (toTiled) std::memcpy(destination + tiledOffset, source + linearOffset, 8);
                else std::memcpy(destination + linearOffset, source + tiledOffset, 8);
            }
            if (x < count) {
                const auto linearOffset = linearBase + x * 4u;
                const auto tiledOffset = tiledBase + (offsets[x] ^ yOffset);
                if constexpr (toTiled) std::memcpy(destination + tiledOffset, source + linearOffset, 4);
                else std::memcpy(destination + linearOffset, source + tiledOffset, 4);
            }
        }
    }
}

}

DepthTargetLayout::DepthTargetLayout(std::uint32_t width, std::uint32_t height) : width(width), height(height) {
    if (width == 0 || height == 0 || width > 16384u || height > 16384u)
        throw std::runtime_error("AGC graphics: invalid depth surface extent");
    blocksPerRow = (width + 127u) / 128u;
    bytes = static_cast<std::size_t>(blocksPerRow) * ((height + 127u) / 128u) * 65536u;
}

std::size_t DepthTargetLayout::offset(std::uint32_t x, std::uint32_t y) const {
    return (static_cast<std::size_t>(y / 128u) * blocksPerRow + x / 128u) * 65536u + localOffset(x, y);
}

std::size_t DepthTargetLayout::Offset(std::uint32_t x, std::uint32_t y) const {
    if (x >= width || y >= height) throw std::runtime_error("AGC graphics: depth coordinate out of range");
    return offset(x, y);
}

void DepthTargetLayout::Detile(std::span<const std::byte> source, std::span<std::byte> destination) const {
    if (source.size() != Bytes() || destination.size() != LinearBytes())
        throw std::runtime_error("AGC graphics: depth detile buffer size mismatch");
    transfer<false>(source.data(), destination.data(), width, height, blocksPerRow);
}

void DepthTargetLayout::Tile(std::span<const std::byte> source, std::span<std::byte> destination) const {
    if (source.size() != LinearBytes() || destination.size() != Bytes())
        throw std::runtime_error("AGC graphics: depth tile buffer size mismatch");
    transfer<true>(source.data(), destination.data(), width, height, blocksPerRow);
}

}
