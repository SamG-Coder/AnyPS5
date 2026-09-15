#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DRAW_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DRAW_HPP

#include "prx/libSceAgcDriver/Graphics/include/Pipeline.hpp"
#include "prx/libSceAgcDriver/Execution/include/Pm4.hpp"

namespace AgcDriver::Graphics {

void DrawIndexed(const Context& context, const State& state, const Pm4::IndexedDraw& draw, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment);

}

#endif
