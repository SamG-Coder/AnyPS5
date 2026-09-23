#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_BUFFERPOOL_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_BUFFERPOOL_HPP

#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include <array>
#include <memory>
#include <mutex>
#include <optional>

namespace AgcDriver::Graphics {

struct BufferAllocation {
    VkBuffer buffer;
    VkDeviceMemory memory;
    void* mapping;
    VkDeviceAddress address;
    VkDeviceSize allocationBytes;
    std::size_t bytes;
    VkBufferUsageFlags usage;
};

class BufferPool {
public:
    explicit BufferPool(const Context& context);
    ~BufferPool();
    BufferPool(const BufferPool&) = delete;
    BufferPool& operator=(const BufferPool&) = delete;
    std::optional<BufferAllocation> Take(std::size_t bytes, VkBufferUsageFlags usage);
    void Put(const BufferAllocation& allocation) noexcept;

private:
    void destroy(const BufferAllocation& allocation) noexcept;
    VkDevice device;
    PFN_vkUnmapMemory unmap;
    PFN_vkDestroyBuffer destroyBuffer;
    PFN_vkFreeMemory freeMemory;
    std::mutex mutex;
    std::array<std::optional<BufferAllocation>, 64> free;
    VkDeviceSize retainedBytes = 0;
    std::size_t cursor = 0;
    static constexpr VkDeviceSize budget = 512ull * 1024 * 1024;
};

std::shared_ptr<BufferPool> GetBufferPool(const Context& context);

}

#endif
