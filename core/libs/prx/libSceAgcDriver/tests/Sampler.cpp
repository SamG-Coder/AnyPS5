#include "GraphicsTests.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Sampler.hpp"
#include <array>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace {

using namespace AgcDriver::Graphics;
VkSamplerCreateInfo captured{};
std::uint32_t created = 0;
std::uint32_t destroyed = 0;

VKAPI_ATTR VkResult VKAPI_CALL createSampler(VkDevice, const VkSamplerCreateInfo* info, const VkAllocationCallbacks*, VkSampler* sampler) {
    captured = *info;
    *sampler = reinterpret_cast<VkSampler>(static_cast<std::uintptr_t>(++created));
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL destroySampler(VkDevice, VkSampler, const VkAllocationCallbacks*) {
    ++destroyed;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL resolve(VkDevice, const char* name) {
    if (std::strcmp(name, "vkCreateSampler") == 0) return reinterpret_cast<PFN_vkVoidFunction>(createSampler);
    if (std::strcmp(name, "vkDestroySampler") == 0) return reinterpret_cast<PFN_vkVoidFunction>(destroySampler);
    return nullptr;
}

}

int main() {
    try {
        RunGuestSamplerResourceTests();
        Context context{};
        context.deviceProc = resolve;
        context.limits.maxSamplerAnisotropy = 16;
        context.limits.maxSamplerLodBias = 16;
        const std::array<std::uint32_t, 4> seamless{0u, 0x00fff000u, 0x09000000u, 0u};
        auto nonSeamless = seamless;
        nonSeamless[0] |= 1u << 28u;
        const auto descriptor = DecodeSamplerResource(nonSeamless);
        bool rejected = false;
        try {
            Sampler sampler(context, descriptor);
        } catch (const std::runtime_error& error) {
            rejected = std::string_view(error.what()).find("device does not support") != std::string_view::npos;
        }
        Require(rejected && created == 0, "unsupported cube filtering reached Vulkan sampler creation");
        {
            Sampler sampler(context, DecodeSamplerResource(seamless));
            Require(captured.flags == 0, "seamless sampler unexpectedly requires an extension");
        }
        context.nonSeamlessCubeMap = true;
        {
            SamplerCache cache;
            auto first = cache.Get(context, nonSeamless, descriptor);
            Require(captured.flags == VK_SAMPLER_CREATE_NON_SEAMLESS_CUBE_MAP_BIT_EXT, "missing non-seamless cube flag");
            Require(captured.mipmapMode == VK_SAMPLER_MIPMAP_MODE_LINEAR && captured.addressModeU == VK_SAMPLER_ADDRESS_MODE_REPEAT,
                "cube filtering changed unrelated sampler settings");
            Require(first == cache.Get(context, nonSeamless, descriptor), "identical sampler was not cached");
            auto second = cache.Get(context, seamless, DecodeSamplerResource(seamless));
            Require(first != second && captured.flags == 0, "cache conflated cube filtering modes");
        }
        Require(created == 3 && destroyed == created, "sampler lifetime mismatch");
        std::puts("Guest sampler tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
