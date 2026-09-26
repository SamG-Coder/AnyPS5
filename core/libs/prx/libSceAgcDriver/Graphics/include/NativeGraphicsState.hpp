#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEGRAPHICSSTATE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEGRAPHICSSTATE_HPP
#include "prx/libSceAgcDriver/Graphics/include/State.hpp"
#include "SceShaders.hpp"
#include <cstdint>
#include <optional>
namespace AgcDriver::Graphics {
class NativeGraphicsState {
public:
    void SetContext(std::uint32_t offset, std::uint32_t value);
    void SetUser(std::uint32_t offset, std::uint32_t value);
    const State& Get() const { return state; }
    std::optional<std::uint32_t> Primitive() const { return primitive; }
private:
    void updateTopology();
    void updateRaster();
    void updateViewport();
    void updateScissor();
    State state{};
    std::optional<std::uint32_t> primitive;
    std::optional<std::uint32_t> raster;
    std::optional<std::uint32_t> viewportControl;
    std::optional<std::uint32_t> clipControl;
    std::optional<std::uint32_t> viewport[6];
    std::optional<std::uint32_t> screenTl, screenBr, windowTl, windowBr;
};
}
#endif
