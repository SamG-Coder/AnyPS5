#include "prx/libSceAgcDriver/Graphics/include/Texture.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

Texture::Texture(const Context& context, TextureDetiler& detiler, const GuestTextureResource& descriptor, VkComponentMapping components) : context(context) {
    static_cast<void>(detiler);
    static_cast<void>(descriptor);
    static_cast<void>(components);
    throw std::runtime_error(std::string(__func__) + " not implemented");
}

Texture::~Texture() {
    release();
}

void Texture::release() noexcept {
    if (view) context.Function<PFN_vkDestroyImageView>("vkDestroyImageView")(context.device, view, nullptr);
    if (image) context.Function<PFN_vkDestroyImage>("vkDestroyImage")(context.device, image, nullptr);
    if (memory) context.Function<PFN_vkFreeMemory>("vkFreeMemory")(context.device, memory, nullptr);
}

VkImageView Texture::View() const {
    return view;
}

}
