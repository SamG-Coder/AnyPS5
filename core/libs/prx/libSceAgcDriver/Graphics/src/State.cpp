#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace AgcDriver::Graphics {
namespace {

std::uint32_t read(const Registers& registers, std::uint32_t offset) {
    const auto it = registers.find(offset);
    Require(it != registers.end(), "missing register at DWORD " + std::to_string(offset));
    return it->second;
}

float readFloat(const Registers& registers, std::uint32_t offset) {
    const auto value = std::bit_cast<float>(read(registers, offset));
    Require(std::isfinite(value), "non-finite register at DWORD " + std::to_string(offset));
    return value;
}

void zero(const Registers& registers, std::uint32_t offset, std::uint32_t mask, const char* name) {
    Require((read(registers, offset) & mask) == 0, std::string(name) + " is unsupported");
}

VkBlendFactor blendFactor(std::uint32_t value) {
    switch (value) {
        case 0: return VK_BLEND_FACTOR_ZERO;
        case 1: return VK_BLEND_FACTOR_ONE;
        case 2: return VK_BLEND_FACTOR_SRC_COLOR;
        case 3: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case 4: return VK_BLEND_FACTOR_SRC_ALPHA;
        case 5: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case 6: return VK_BLEND_FACTOR_DST_ALPHA;
        case 7: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case 8: return VK_BLEND_FACTOR_DST_COLOR;
        case 9: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case 10: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case 13: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case 14: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case 19: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case 20: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        default: throw std::runtime_error("AGC graphics: unsupported blend factor " + std::to_string(value));
    }
}

VkBlendOp blendOp(std::uint32_t value) {
    switch (value) {
        case 0: return VK_BLEND_OP_ADD;
        case 1: return VK_BLEND_OP_SUBTRACT;
        case 2: return VK_BLEND_OP_MIN;
        case 3: return VK_BLEND_OP_MAX;
        case 4: return VK_BLEND_OP_REVERSE_SUBTRACT;
        default: throw std::runtime_error("AGC graphics: unsupported blend operation " + std::to_string(value));
    }
}

void intersect(VkRect2D& result, const Registers& registers, std::uint32_t offset, bool screen) {
    const auto tl = read(registers, offset);
    const auto br = read(registers, offset + 1);
    if (!screen) Require((tl & 0x80008000u) == 0x80000000u && (br & 0x80008000u) == 0, "scissor window offsets or reserved bits are unsupported");
    const auto x = tl & 0xffffu;
    const auto y = (tl >> 16u) & (screen ? 0xffffu : 0x7fffu);
    const auto right = br & 0xffffu;
    const auto bottom = br >> 16u;
    Require(x <= right && y <= bottom, "inverted scissor rectangle");
    const auto oldRight = static_cast<std::uint32_t>(result.offset.x) + result.extent.width;
    const auto oldBottom = static_cast<std::uint32_t>(result.offset.y) + result.extent.height;
    const auto left = std::max(static_cast<std::uint32_t>(result.offset.x), x);
    const auto top = std::max(static_cast<std::uint32_t>(result.offset.y), y);
    result.offset = {static_cast<std::int32_t>(left), static_cast<std::int32_t>(top)};
    result.extent = {std::min(oldRight, right) > left ? std::min(oldRight, right) - left : 0, std::min(oldBottom, bottom) > top ? std::min(oldBottom, bottom) - top : 0};
}

}

State DecodeState(const QueueState& queue) {
    const auto& cx = queue.context;
    State result{};
    const auto primitive = read(queue.userConfig, 0x242);
    switch (primitive) {
        case 4: result.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case 5: result.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
        case 6: result.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        default: throw std::runtime_error("AGC graphics: unsupported primitive type " + std::to_string(primitive));
    }
    zero(cx, 0x2a5, ~0u, "primitive restart");
    zero(cx, 0x2e5, ~0u, "stream output");
    zero(cx, 0x2e6, ~0u, "stream output buffers");
    Require(read(cx, 0x2d5) == 0x2000u, "only a wave64 primitive-generation vertex stage without tessellation or geometry amplification is supported");
    zero(cx, 0x1b6, 0x8000u, "wave32 fragment shaders");
    zero(cx, 0x207, ~0u, "clip distances, layer, viewport or auxiliary vertex exports");
    zero(cx, 0x200, ~0x007007f0u, "depth, stencil or conditional color writes");
    zero(cx, 0x203, ~0x00009870u, "depth export, shader coverage or ordered fragment execution");
    zero(cx, 0x2dc, ~0x0001ff00u, "alpha-to-coverage");
    zero(cx, 0x2f8, ~0u, "multisampling or coverage conversion");
    zero(cx, 0x292, ~2u, "scan conversion mode");
    zero(cx, 0x293, ~0x06003fffu, "sample iteration, primitive discard or out-of-order rasterization");
    zero(cx, 0x80, ~0u, "window offset");
    zero(cx, 0x8d, ~0u, "hardware screen offset");
    Require(read(cx, 0x83) == 0xffffu, "clip rectangles are unsupported");
    Require((read(cx, 0x8c) & 0xfu) == 0xau, "nonstandard triangle edge rules are unsupported");
    Require(read(cx, 0x2f9) == 0x2du, "nonstandard pixel center or vertex quantization is unsupported");
    Require(read(cx, 0x313) == 0x6000u, "conservative rasterization is unsupported");
    Require(read(cx, 0x30e) == 0xffffffffu && read(cx, 0x30f) == 0xffffffffu, "sample masks are unsupported");
    Require(read(cx, 0x206) == 0x3fu, "only homogeneous positions with all viewport transforms enabled are supported");
    Require(read(cx, 0x204) == 0x80000u, "only standard zero-to-one depth clipping is supported");
    const auto raster = read(cx, 0x205);
    Require((raster & ~0x7u) == 0 || (raster & ~0x7u) == 0x240u, "polygon mode, depth bias, provoking vertex or nonstandard rasterization is unsupported");
    result.cullMode = ((raster & 1u) != 0 ? VK_CULL_MODE_FRONT_BIT : 0u) | ((raster & 2u) != 0 ? VK_CULL_MODE_BACK_BIT : 0u);
    result.frontFace = (raster & 4u) != 0 ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    const auto targetMask = read(cx, 0x8e);
    const auto shaderMask = read(cx, 0x8f);
    Require(targetMask != 0 && (targetMask & ~0xfu) == 0 && (shaderMask & ~0xfu) == 0, "only color target zero is supported");
    Require(shaderMask == 0xfu, "partial shader color exports are unsupported");
    Require(read(cx, 0x202) == 0xcc0010u, "only normal color rendering with copy ROP is supported");
    zero(cx, 0x1c4, ~0u, "depth or sample-mask export");
    const auto exportFormat = read(cx, 0x1c5);
    Require(exportFormat == 4 || exportFormat == 9, "only FP16_ABGR or 32_ABGR color export is supported");
    Require(read(cx, 0x1c3) == 4, "additional position exports are unsupported");
    const auto info = read(cx, 0x31c);
    const auto number = (info >> 8u) & 7u;
    const auto swap = (info >> 11u) & 3u;
    Require(((info >> 2u) & 0x1fu) == 10 && (number == 0 || number == 6) && swap <= 1, "unsupported color format or component order");
    Require((info & ~0x00029f7cu) == 0, "color compression, DCC, endian conversion, nonstandard rounding or color optimization is unsupported");
    Require((info & 0x8000u) != 0, "unclamped normalized color is unsupported");
    zero(cx, 0x31b, ~0u, "color mip or array view");
    zero(cx, 0x31d, ~0u, "color samples, fragments or destination alpha override");
    const auto attrib2 = read(cx, 0x3b0);
    Require((attrib2 >> 28u) == 0, "mipmapped render targets are unsupported");
    const auto attrib3 = read(cx, 0x3b8);
    Require((attrib3 & ~0x44000000u) == 0x09000000u, "only linear, non-array 2D color surfaces with resource level one are supported");
    result.color.extent = {((attrib2 >> 14u) & 0x3fffu) + 1u, (attrib2 & 0x3fffu) + 1u};
    Require(result.color.extent.width % 64u == 0, "linear surface pitch cannot be inferred for widths not aligned to 256 bytes");
    const auto high = read(cx, 0x390);
    Require((high & ~0xffu) == 0, "invalid color address extension");
    result.color.address = (static_cast<std::uint64_t>(high) << 40u) | (static_cast<std::uint64_t>(read(cx, 0x318)) << 8u);
    const auto bytes = static_cast<std::uint64_t>(result.color.extent.width) * result.color.extent.height * 4u;
    Require(bytes <= std::numeric_limits<std::size_t>::max(), "color surface size overflow");
    result.color.bytes = static_cast<std::size_t>(bytes);
    GuestMemory::CheckRange(reinterpret_cast<const void*>(result.color.address), result.color.bytes, 256, true);
    result.color.format = swap == 0 ? (number == 0 ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB) : (number == 0 ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_B8G8R8A8_SRGB);
    const auto xs = readFloat(cx, 0x10f);
    const auto xo = readFloat(cx, 0x110);
    const auto ys = readFloat(cx, 0x111);
    const auto yo = readFloat(cx, 0x112);
    const auto zs = readFloat(cx, 0x113);
    const auto zo = readFloat(cx, 0x114);
    Require(xs > 0 && ys != 0 && zo >= 0 && zo <= 1 && zo + zs >= 0 && zo + zs <= 1, "unsupported viewport transform");
    Require(readFloat(cx, 0xb4) == std::min(zo, zo + zs) && readFloat(cx, 0xb5) == std::max(zo, zo + zs), "viewport depth clamp differs from transform");
    result.viewport = {xo - xs, yo - ys, 2 * xs, 2 * ys, zo, zo + zs};
    result.scissor = {{0, 0}, result.color.extent};
    intersect(result.scissor, cx, 0xc, true);
    intersect(result.scissor, cx, 0x81, false);
    intersect(result.scissor, cx, 0x90, false);
    if ((read(cx, 0x292) & 2u) != 0) intersect(result.scissor, cx, 0x94, false);
    const auto blend = read(cx, 0x1e0);
    Require((blend & 0x0000e000u) == 0, "reserved blend control bits");
    result.blend.colorWriteMask = targetMask;
    result.blend.blendEnable = (blend >> 30u) & 1u;
    if (result.blend.blendEnable) {
        Require((info & 0x10000u) == 0, "blend bypass conflicts with enabled blending");
        result.blend.srcColorBlendFactor = blendFactor(blend & 0x1fu);
        result.blend.dstColorBlendFactor = blendFactor((blend >> 8u) & 0x1fu);
        result.blend.colorBlendOp = blendOp((blend >> 5u) & 7u);
        const auto alpha = (blend & 0x20000000u) != 0 ? blend >> 16u : blend;
        result.blend.srcAlphaBlendFactor = blendFactor(alpha & 0x1fu);
        result.blend.dstAlphaBlendFactor = blendFactor((alpha >> 8u) & 0x1fu);
        result.blend.alphaBlendOp = blendOp((alpha >> 5u) & 7u);
        for (std::uint32_t i = 0; i < 4; ++i) result.blendConstants[i] = readFloat(cx, 0x105 + i);
    }
    return result;
}

}
