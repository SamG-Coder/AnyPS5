#include "prx/libSceAgcDriver/Graphics/include/TextureDetiler.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

TextureDetiler::TextureDetiler(const Context& context) : context(context) {
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

TextureDetiler::~TextureDetiler() {
    release();
}

void TextureDetiler::release() noexcept {
}

VkPipeline TextureDetiler::pipeline(TextureTileMode tileMode, std::uint32_t elementBytes) {
    static_cast<void>(tileMode);
    static_cast<void>(elementBytes);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

void TextureDetiler::Dispatch(VkCommandBuffer commands, TextureTileMode tileMode, std::uint32_t elementBytes, VkBuffer source, std::uint64_t sourceOffset, VkBuffer destination, std::uint64_t destinationOffset, const TileMipLayout& layout) {
    static_cast<void>(commands);
    static_cast<void>(tileMode);
    static_cast<void>(elementBytes);
    static_cast<void>(source);
    static_cast<void>(sourceOffset);
    static_cast<void>(destination);
    static_cast<void>(destinationOffset);
    static_cast<void>(layout);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

}
