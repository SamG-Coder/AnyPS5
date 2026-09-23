#include "prx/libSceAgcDriver/Graphics/include/Sampler.hpp"

namespace AgcDriver::Graphics {

    Sampler::Sampler(const Context& context, const GuestSamplerResource& descriptor) : context(context) {
        Require(!descriptor.anisotropyEnable || context.samplerAnisotropy, "guest sampler descriptor requests anisotropic filtering which the device does not support");
        Require(descriptor.maxAnisotropy <= context.limits.maxSamplerAnisotropy, "guest sampler descriptor requests an anisotropy ratio beyond the device limit");
        Require(descriptor.lodBias >= -context.limits.maxSamplerLodBias && descriptor.lodBias <= context.limits.maxSamplerLodBias, "guest sampler descriptor requests a LOD bias beyond the device limit");

        VkSamplerCreateInfo info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
        info.magFilter = descriptor.magFilter;
        info.minFilter = descriptor.minFilter;
        info.mipmapMode = descriptor.mipmapMode;
        info.addressModeU = descriptor.addressModeU;
        info.addressModeV = descriptor.addressModeV;
        info.addressModeW = descriptor.addressModeW;
        info.mipLodBias = descriptor.lodBias;
        info.anisotropyEnable = descriptor.anisotropyEnable ? VK_TRUE : VK_FALSE;
        info.maxAnisotropy = descriptor.maxAnisotropy;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_NEVER;
        info.minLod = descriptor.minLod;
        info.maxLod = descriptor.maxLod;
        info.borderColor = descriptor.borderColor;
        info.unnormalizedCoordinates = VK_FALSE;
        Check(context.Function<PFN_vkCreateSampler>("vkCreateSampler")(context.device, &info, nullptr, &sampler), "vkCreateSampler");
    }

    Sampler::~Sampler() {
        release();
    }

    void Sampler::release() noexcept {
        if (sampler) context.Function<PFN_vkDestroySampler>("vkDestroySampler")(context.device, sampler, nullptr);
    }

    VkSampler Sampler::Handle() const {
        return sampler;
    }

}
