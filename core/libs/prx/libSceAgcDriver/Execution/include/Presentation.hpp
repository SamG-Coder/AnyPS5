#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PRESENTATION_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PRESENTATION_HPP

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <cstdint>
#include <exception>
#include <span>
#include "prx/libSceAgcDriver/Execution/include/DisplayBuffer.hpp"

namespace AgcDriver {

struct PresentationWindow {
    void* context;
    std::span<const char* const> extensions;
    VkSurfaceKHR (*createSurface)(void* context, VkInstance instance);
    std::uint32_t width;
    std::uint32_t height;
};

}

extern "C" void AgcDriverPresentClear_nid_postfix(const AgcDriver::PresentationWindow& window, bool opaque, void (*gpuReady)(void*), void* context);
extern "C" void AgcDriverPresentBuffer_nid_postfix(const AgcDriver::PresentationWindow& window, const AgcDriver::DisplayBuffer& buffer, void (*gpuReady)(void*), void* context);
extern "C" void AgcDriverReleaseWindow_nid_postfix(void* window);
extern "C" void AgcDriverReportFailure_nid_postfix(std::exception_ptr error);

#endif
