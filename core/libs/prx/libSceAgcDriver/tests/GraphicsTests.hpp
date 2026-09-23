#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_TESTS_GRAPHICSTESTS_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_TESTS_GRAPHICSTESTS_HPP

#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

struct DetilerCapture {
    std::uint32_t groupsX;
    std::uint32_t groupsY;
    std::uint32_t groupsZ;
    std::vector<std::byte> pushConstants;
    VkBuffer sourceBuffer;
    VkDeviceSize sourceOffset;
    VkDeviceSize sourceRange;
    VkBuffer destinationBuffer;
    VkDeviceSize destinationOffset;
    VkDeviceSize destinationRange;
};

struct TextureDetilerTestAccess {
    std::function<VkBuffer(std::uint64_t)> makeBuffer;
    std::function<std::vector<std::byte>&(VkBuffer)> bytes;
    std::function<DetilerCapture()> lastDispatch;
    std::function<std::uint32_t()> pipelineCount;
};

void RunTextureFormatTests();
void RunTextureTilingTests();
void RunGuestTextureResourceTests();
void RunGuestSamplerResourceTests();
void RunTextureDetilerTests(const AgcDriver::Graphics::Context& context, const TextureDetilerTestAccess& access);

#endif
