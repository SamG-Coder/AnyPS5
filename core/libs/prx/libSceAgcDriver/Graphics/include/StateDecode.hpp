#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_STATEDECODE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_STATEDECODE_HPP
#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "prx/libSceAgcDriver/Execution/include/QueueState.hpp"
namespace AgcDriver::Graphics {
ShaderStages DecodeShaderStages(const QueueState& queue);
State DecodeState(const QueueState& queue);
}
#endif
