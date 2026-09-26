#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSURFACE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSURFACE_HPP

#include "prx/libSceAgcDriver/Graphics/include/Resources.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"

namespace AgcDriver::Graphics {

class DepthSurface {
public:
    DepthSurface(const Context& context, const DepthState& state);
    VkImageView View() const { return target.View(); }
    VkExtent2D Extent() const { return extent; }
    void Upload(VkCommandBuffer commands, std::span<const std::byte> tiled);
    void Download(VkCommandBuffer commands);
    void Read(std::span<std::byte> tiled);

private:
    void transition(VkCommandBuffer commands, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags sourceAccess, VkAccessFlags destinationAccess,
        VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage);
    VkBufferImageCopy region() const;
    Context context;
    VkExtent2D extent;
    DepthTargetLayout layout;
    RenderTarget target;
    Buffer transfer;
};

}

#endif
