#include "prx/libSceAgcDriver/Graphics/include/GuestTextureResource.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

GuestTextureResource DecodeTextureResource(std::span<const std::uint32_t> words) {
    static_cast<void>(words);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

bool MatchesGuestDimension(ShaderRecompiler::DescriptorImageShape shape, TextureDimension dimension) {
    static_cast<void>(shape);
    static_cast<void>(dimension);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

}
