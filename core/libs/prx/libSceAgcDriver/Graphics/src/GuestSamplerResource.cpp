#include "prx/libSceAgcDriver/Graphics/include/GuestSamplerResource.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

GuestSamplerResource DecodeSamplerResource(std::span<const std::uint32_t> words) {
    static_cast<void>(words);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

}
