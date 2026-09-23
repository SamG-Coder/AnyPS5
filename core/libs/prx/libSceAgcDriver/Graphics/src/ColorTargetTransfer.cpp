#include "prx/libSceAgcDriver/Graphics/include/ColorTargetTransfer.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <vector>

namespace AgcDriver::Graphics {

void ReadColorTarget(const ColorTarget& target, std::span<std::byte> destination) {
    const ColorTargetLayout layout(target.extent.width, target.extent.height, target.tileMode);
    Require(target.bytes == layout.Bytes() && destination.size() == layout.LinearBytes(), "color target transfer size mismatch");
    if (target.tileMode == ColorTileMode::Linear) {
        GuestMemory::Read(target.address, destination, layout.Alignment());
        return;
    }
    std::vector<std::byte> tiled(layout.Bytes());
    GuestMemory::Read(target.address, tiled, layout.Alignment());
    layout.Detile(tiled, destination);
}

void WriteColorTarget(const ColorTarget& target, std::span<const std::byte> source) {
    const ColorTargetLayout layout(target.extent.width, target.extent.height, target.tileMode);
    Require(target.bytes == layout.Bytes() && source.size() == layout.LinearBytes(), "color target transfer size mismatch");
    if (target.tileMode == ColorTileMode::Linear) {
        GuestMemory::Write(target.address, source, layout.Alignment());
        return;
    }
    std::vector<std::byte> tiled(layout.Bytes());
    GuestMemory::Read(target.address, tiled, layout.Alignment());
    layout.Tile(source, tiled);
    GuestMemory::Write(target.address, tiled, layout.Alignment());
}

}
