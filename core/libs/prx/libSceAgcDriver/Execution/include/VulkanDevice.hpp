#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VULKANDEVICE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VULKANDEVICE_HPP

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include "shader/recompilier/Recompiler.hpp"
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
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
    std::uint64_t PresentClear(std::uint32_t width, std::uint32_t height, bool opaque);
    void WaitPresented(std::uint64_t id);
    void Dispatch(const ShaderRecompiler::RecompileResult& shader, std::uint32_t x, std::uint32_t y, std::uint32_t z);

private:
    struct State;
    std::unique_ptr<State> state;
};

}

#endif
