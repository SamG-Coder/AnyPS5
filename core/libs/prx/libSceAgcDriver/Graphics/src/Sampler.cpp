#include "prx/libSceAgcDriver/Graphics/include/Sampler.hpp"
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

Sampler::Sampler(const Context& context, const GuestSamplerResource& descriptor) : context(context) {
    static_cast<void>(descriptor);
    throw std::runtime_error(std::string(__func__) + " not implemented");
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
