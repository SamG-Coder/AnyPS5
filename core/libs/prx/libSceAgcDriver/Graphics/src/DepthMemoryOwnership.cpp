#include "prx/libSceAgcDriver/Graphics/include/DepthSurface.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/MemoryAccessScope.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include "prx/libc/include/GuestMemoryBacking.hpp"
#include <limits>

namespace AgcDriver::Graphics {

void DepthSurface::BindGuest(std::uint64_t address, std::function<void(bool)> waitForDraws) {
    Require(address != 0 && address % 65536 == 0 && layout.Bytes() <= std::numeric_limits<std::uint64_t>::max() - address, "invalid resident depth range");
    if (watch && guestAddress == address) return;
    ReleaseGuest();
    Require(static_cast<bool>(waitForDraws), "missing depth synchronization callback");
    auto replacement = std::make_unique<GuestMemoryTracking::Watch>(address, layout.Bytes(), this,
        [](void* owner, GuestMemoryTracking::Access access) { static_cast<DepthSurface*>(owner)->resolveGuest(access); });
    guestBytes.resize(layout.Bytes());
    wait = std::move(waitForDraws);
    guestAddress = address;
    watch = std::move(replacement);
}

void DepthSurface::BeginGuest(VkCommandBuffer commands, bool writable) {
    PerformanceTimer timing("Graphics.Depth.Begin");
    Require(watch != nullptr, "depth surface has no guest owner");
    if (!valid) {
        watch->Protect(GuestMemoryTracking::Protection::Read);
        const GuestMemory::MemoryAccessScope suspended(nullptr, nullptr);
        GuestMemory::Read(guestAddress, guestBytes, 65536);
        Upload(commands, guestBytes);
        valid = true;
        timing.Mark("upload", guestBytes.size());
    } else {
        transition(commands, imageLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
        timing.Mark("reuse");
    }
    dirty = dirty || writable;
    watch->Protect(dirty ? GuestMemoryTracking::Protection::None : GuestMemoryTracking::Protection::Read);
}

void DepthSurface::resolveGuest(GuestMemoryTracking::Access access) {
    PerformanceTimer timing("Graphics.Depth.Resolve");
    Require(watch != nullptr && static_cast<bool>(wait), "depth ownership resolver is unavailable");
    wait(true);
    timing.Mark("draw_wait");
    if (dirty) {
        CommandBatch batch(context);
        Download(batch.Handle());
        batch.SubmitAndWait();
        Read(guestBytes);
        GuestMemoryBacking::GuestMemoryBackingWrite_nid_postfix(guestAddress, guestBytes.data(), guestBytes.size());
        dirty = false;
        timing.Mark("writeback", guestBytes.size());
    }
    if (access != GuestMemoryTracking::Access::Read) valid = false;
    watch->Protect(valid ? GuestMemoryTracking::Protection::Read : GuestMemoryTracking::Protection::ReadWrite);
    if (access == GuestMemoryTracking::Access::Invalidate) wait(false);
}

void DepthSurface::ReleaseGuest() {
    if (!watch) return;
    resolveGuest(GuestMemoryTracking::Access::Invalidate);
    watch.reset();
    guestAddress = 0;
    wait = {};
}

bool DepthSurface::SharesPages(std::uint64_t address, std::size_t bytes) const {
    if (!watch || bytes == 0) return false;
    Require(bytes <= std::numeric_limits<std::uint64_t>::max() - address, "depth overlap range overflow");
    const auto page = GuestMemoryTracking::GuestMemoryTrackingPageSize_nid_postfix();
    return guestAddress / page <= (address + bytes - 1) / page && address / page <= (guestAddress + layout.Bytes() - 1) / page;
}

}
