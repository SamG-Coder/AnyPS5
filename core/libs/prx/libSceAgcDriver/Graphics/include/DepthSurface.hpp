#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSURFACE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSURFACE_HPP

#include "prx/libSceAgcDriver/Graphics/include/Resources.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include "prx/libc/include/GuestMemoryTracking.hpp"
#include <functional>
#include <vector>

namespace AgcDriver::Graphics {

class DepthSurface {
public:
    DepthSurface(const Context& context, const DepthState& state);
    VkImageView View() const { return target.View(); }
    VkExtent2D Extent() const { return extent; }
    void Upload(VkCommandBuffer commands, std::span<const std::byte> tiled);
    void Download(VkCommandBuffer commands);
    void Read(std::span<std::byte> tiled);
    void BindGuest(std::uint64_t address, std::function<void(bool)> wait);
    void BeginGuest(VkCommandBuffer commands, bool writable);
    void ReleaseGuest();
    bool SharesPages(std::uint64_t address, std::size_t bytes) const;

private:
    void transition(VkCommandBuffer commands, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags sourceAccess, VkAccessFlags destinationAccess,
        VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage);
    VkBufferImageCopy region() const;
    void resolveGuest(GuestMemoryTracking::Access access);
    Context context;
    VkExtent2D extent;
    DepthTargetLayout layout;
    RenderTarget target;
    Buffer transfer;
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    std::uint64_t guestAddress = 0;
    bool valid = false;
    bool dirty = false;
    std::vector<std::byte> guestBytes;
    std::function<void(bool)> wait;
    std::unique_ptr<GuestMemoryTracking::Watch> watch;
};

}

#endif
