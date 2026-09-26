#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSTATE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSTATE_HPP

#include "prx/libSceAgcDriver/Execution/include/QueueState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include <cstddef>
#include <optional>

namespace AgcDriver::Graphics {

struct DepthState {
    std::uint64_t address;
    VkExtent2D extent;
    std::size_t bytes;
    VkCompareOp compare;
    bool writeEnabled;
};

std::optional<DepthState> DecodeDepthState(const Registers& registers);

}

#endif
