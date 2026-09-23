#include "prx/libSceAgcDriver/Graphics/include/TextureTiling.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

std::vector<TileMipLayout> ComputeMipLayout(TextureTileMode tileMode, std::uint32_t format, std::uint32_t width, std::uint32_t height, std::uint32_t mipCount) {
    static_cast<void>(tileMode);
    static_cast<void>(format);
    static_cast<void>(width);
    static_cast<void>(height);
    static_cast<void>(mipCount);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

std::uint64_t ComputeSurfaceSize(const std::vector<TileMipLayout>& mips, std::uint32_t arrayLayers) {
    static_cast<void>(mips);
    static_cast<void>(arrayLayers);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

}
