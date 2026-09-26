#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include <cstring>
#include <stdexcept>

namespace AgcDriver::Graphics {

DepthTargetLayout::DepthTargetLayout(std::uint32_t width, std::uint32_t height) : width(width), height(height) {
    if (width == 0 || height == 0 || width > 16384u || height > 16384u)
        throw std::runtime_error("AGC graphics: invalid depth surface extent");
    blocksPerRow = (width + 127u) / 128u;
    bytes = static_cast<std::size_t>(blocksPerRow) * ((height + 127u) / 128u) * 65536u;
}

std::size_t DepthTargetLayout::offset(std::uint32_t x, std::uint32_t y) const {
    const auto bit = [](std::uint32_t value, unsigned source, unsigned destination) {
        return ((value >> source) & 1u) << destination;
    };
    const auto local = bit(x, 0, 2) | bit(y, 0, 3) | bit(x, 1, 4) | bit(y, 1, 5) |
        bit(x, 2, 6) | bit(y, 2, 7) | bit(x ^ y, 3, 8) | bit(x ^ y, 4, 9) |
        (((y >> 5u ^ x >> 6u) & 1u) << 10u) | (((x >> 5u ^ y >> 6u) & 1u) << 11u) |
        bit(y, 3, 12) | bit(x, 4, 13) | bit(y, 6, 14) | bit(x, 6, 15);
    return (static_cast<std::size_t>(y / 128u) * blocksPerRow + x / 128u) * 65536u + local;
}

std::size_t DepthTargetLayout::Offset(std::uint32_t x, std::uint32_t y) const {
    if (x >= width || y >= height) throw std::runtime_error("AGC graphics: depth coordinate out of range");
    return offset(x, y);
}

void DepthTargetLayout::Detile(std::span<const std::byte> source, std::span<std::byte> destination) const {
    if (source.size() != Bytes() || destination.size() != LinearBytes())
        throw std::runtime_error("AGC graphics: depth detile buffer size mismatch");
    for (std::uint32_t y = 0; y < height; ++y)
        for (std::uint32_t x = 0; x < width; ++x)
            std::memcpy(destination.data() + (static_cast<std::size_t>(y) * width + x) * 4u, source.data() + offset(x, y), 4);
}

void DepthTargetLayout::Tile(std::span<const std::byte> source, std::span<std::byte> destination) const {
    if (source.size() != LinearBytes() || destination.size() != Bytes())
        throw std::runtime_error("AGC graphics: depth tile buffer size mismatch");
    for (std::uint32_t y = 0; y < height; ++y)
        for (std::uint32_t x = 0; x < width; ++x)
            std::memcpy(destination.data() + offset(x, y), source.data() + (static_cast<std::size_t>(y) * width + x) * 4u, 4);
}

}