#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_STATE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_STATE_HPP

#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include "prx/libSceAgcDriver/Execution/include/QueueState.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace AgcDriver::Graphics {

struct ColorTarget {
    std::uint64_t address;
    VkExtent2D extent;
    VkFormat format;
    std::size_t bytes;
};

struct State {
    ColorTarget color;
    VkPrimitiveTopology topology;
    VkViewport viewport;
    VkRect2D scissor;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    VkPipelineColorBlendAttachmentState blend;
    std::array<float, 4> blendConstants;
};

State DecodeState(const QueueState& queue);

}

#endif
