#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VULKANDEVICE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VULKANDEVICE_HPP

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "Recompiler.hpp"
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Draw.hpp"
#include <memory>

namespace AgcDriver {

class VulkanDevice {
public:
    explicit VulkanDevice(const PresentationWindow* window = nullptr);
    ~VulkanDevice();
    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    ShaderRecompiler::SpirvTarget Target() const;
    void WaitIdle();
    void* Window() const;
    void Resize(std::uint32_t width, std::uint32_t height);
    std::uint64_t PresentClear(std::uint32_t width, std::uint32_t height, bool opaque);
    std::uint64_t PresentPixels(std::uint32_t width, std::uint32_t height, std::span<const std::byte> pixels);
    void WaitPresented(std::uint64_t id);
    void Dispatch(const ShaderRecompiler::RecompileResult& shader, std::uint32_t x, std::uint32_t y, std::uint32_t z);
    void DrawIndexed(const Graphics::State& graphics, const Pm4::IndexedDraw& draw, std::span<const Graphics::CompiledShader> shaders);

private:
    std::uint64_t present(std::uint32_t width, std::uint32_t height, bool opaque, std::span<const std::byte> pixels);
    struct State;
    std::unique_ptr<State> state;
};

}

#endif
