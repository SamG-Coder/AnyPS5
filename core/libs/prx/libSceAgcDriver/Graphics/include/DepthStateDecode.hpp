#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSTATEDECODE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DEPTHSTATEDECODE_HPP
#include "prx/libSceAgcDriver/Graphics/include/DepthState.hpp"
#include "prx/libSceAgcDriver/Execution/include/QueueState.hpp"
namespace AgcDriver::Graphics { std::optional<DepthState> DecodeDepthState(const Registers& registers); }
#endif
