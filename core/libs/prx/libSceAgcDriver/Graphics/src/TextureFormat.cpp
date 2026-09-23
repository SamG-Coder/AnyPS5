#include "prx/libSceAgcDriver/Graphics/include/TextureFormat.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

VkFormat ResolveTextureFormat(std::uint32_t guestFormat) {
    static_cast<void>(guestFormat);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

std::uint32_t BytesPerElement(std::uint32_t guestFormat) {
    static_cast<void>(guestFormat);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

bool IsBlockCompressed(std::uint32_t guestFormat) {
    static_cast<void>(guestFormat);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

std::uint32_t BlockWidth(std::uint32_t guestFormat) {
    static_cast<void>(guestFormat);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

std::uint32_t BlockHeight(std::uint32_t guestFormat) {
    static_cast<void>(guestFormat);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

}
