#include "prx/libSceAgcDriver/Graphics/include/DepthState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include <sstream>

namespace AgcDriver::Graphics {

std::optional<DepthState> DecodeDepthState(const Registers& registers) {
    const auto read = [&](std::uint32_t offset) {
        const auto it = registers.find(offset);
        if (it == registers.end()) {
            std::ostringstream message;
            message << "missing depth register at context DWORD 0x" << std::hex << offset;
            Require(false, message.str());
        }
        return it->second;
    };
    const auto control = read(0x200);
    Require((control & ~0x007007f6u) == 0, "stencil, depth bounds or conditional color writes are unsupported");
    if ((control & 2u) == 0) return std::nullopt;
    Require(read(0x0) == 0, "depth clear, copy, decompression or special render controls are unsupported");
    Require(read(0x2) == 0, "depth mip, array or read-only views are unsupported");
    Require(read(0x10) == 0x80000183u, "only single-sample D32 64KB_Z_X depth surfaces are supported");
    const auto size = read(0x7);
    Require((size & 0xc000c000u) == 0, "reserved depth extent bits are set");
    const VkExtent2D extent{(size & 0x3fffu) + 1u, ((size >> 16u) & 0x3fffu) + 1u};
    const auto address = [&](std::uint32_t lowRegister, std::uint32_t highRegister) {
        const auto high = read(highRegister);
        Require((high & ~0xffu) == 0, "invalid depth address extension");
        const auto value = (static_cast<std::uint64_t>(high) << 40u) |
            (static_cast<std::uint64_t>(read(lowRegister)) << 8u);
        Require(value != 0 && (value & 0xffffu) == 0, "depth surface requires a nonzero 64KB-aligned address");
        return value;
    };
    const auto readAddress = address(0x12, 0x1a);
    const bool writeEnabled = (control & 4u) != 0;
    if (writeEnabled)
        Require(address(0x14, 0x1c) == readAddress, "separate depth read and write surfaces are unsupported");
    constexpr VkCompareOp comparisons[]{VK_COMPARE_OP_NEVER, VK_COMPARE_OP_LESS, VK_COMPARE_OP_EQUAL,
        VK_COMPARE_OP_LESS_OR_EQUAL, VK_COMPARE_OP_GREATER, VK_COMPARE_OP_NOT_EQUAL,
        VK_COMPARE_OP_GREATER_OR_EQUAL, VK_COMPARE_OP_ALWAYS};
    return DepthState{readAddress, extent, DepthTargetLayout(extent.width, extent.height).Bytes(),
        comparisons[(control >> 4u) & 7u], writeEnabled};
}

}
