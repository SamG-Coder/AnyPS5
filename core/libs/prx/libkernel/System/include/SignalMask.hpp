#pragma once
#include <array>
#include <cstdint>
namespace GuestSignals {
using Mask = std::array<std::uint32_t, 4>;
Mask CaptureMask();
void InheritMask(const Mask& mask);
}
