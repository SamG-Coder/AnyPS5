#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_CONTEXT_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_CONTEXT_HPP

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace AgcDriver::Graphics {

inline void Require(bool condition, const std::string& reason) {
    if (!condition) throw std::runtime_error("AGC graphics: " + reason);
}

inline void Check(VkResult result, const char* operation) {
    Require(result == VK_SUCCESS, std::string(operation) + ": Vulkan result " + std::to_string(result));
}

struct Context {
    VkDevice device;
    VkPhysicalDevice physical;
    VkQueue queue;
    VkCommandPool pool;
    PFN_vkGetDeviceProcAddr deviceProc;
    PFN_vkGetPhysicalDeviceFormatProperties formatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties imageFormatProperties;
    VkPhysicalDeviceMemoryProperties memory;
    VkPhysicalDeviceLimits limits;
    bool tessellationShader = false;
    bool meshShader = false;
    VkPhysicalDeviceMeshShaderPropertiesEXT meshLimits{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT};
    bool depthClipControl = false;
    bool depthRangeUnrestricted = false;
    bool bufferDeviceAddress = false;
    VkPhysicalDeviceSubgroupProperties subgroup{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};
    bool fragmentShaderBarycentric = false;

    template<typename TFunction>
    TFunction Function(const char* name) const {
        Require(deviceProc != nullptr, "missing Vulkan device function resolver");
        const auto function = reinterpret_cast<TFunction>(deviceProc(device, name));
        Require(function != nullptr, std::string("missing Vulkan function: ") + name);
        return function;
    }

    std::uint32_t MemoryType(std::uint32_t mask, VkMemoryPropertyFlags flags) const {
        for (std::uint32_t i = 0; i < memory.memoryTypeCount; ++i) {
            if ((mask & (1u << i)) != 0 && (memory.memoryTypes[i].propertyFlags & flags) == flags) return i;
        }
        throw std::runtime_error("AGC graphics: required Vulkan memory type is unavailable");
    }
};

}

#endif
