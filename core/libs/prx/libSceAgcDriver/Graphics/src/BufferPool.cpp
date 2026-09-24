#include "prx/libSceAgcDriver/Graphics/include/BufferPool.hpp"

namespace AgcDriver::Graphics {

BufferPool::BufferPool(const Context& context) : device(context.device), unmap(context.Function<PFN_vkUnmapMemory>("vkUnmapMemory")), destroyBuffer(context.Function<PFN_vkDestroyBuffer>("vkDestroyBuffer")), freeMemory(context.Function<PFN_vkFreeMemory>("vkFreeMemory")) {}

BufferPool::~BufferPool() {
    for (const auto& allocation : free) {
        if (allocation) destroy(*allocation);
    }
}

void BufferPool::destroy(const BufferAllocation& allocation) noexcept {
    unmap(device, allocation.memory);
    destroyBuffer(device, allocation.buffer, nullptr);
    freeMemory(device, allocation.memory, nullptr);
}

std::optional<BufferAllocation> BufferPool::Take(std::size_t bytes, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    std::lock_guard lock(mutex);
    for (auto& allocation : free) {
        if (!allocation || allocation->bytes != bytes || allocation->usage != usage || allocation->properties != properties) continue;
        auto result = allocation;
        retainedBytes -= allocation->allocationBytes;
        allocation.reset();
        return result;
    }
    return std::nullopt;
}

void BufferPool::Put(const BufferAllocation& allocation) noexcept {
    std::lock_guard lock(mutex);
    if (allocation.allocationBytes > budget) {
        destroy(allocation);
        return;
    }
    while (retainedBytes + allocation.allocationBytes > budget || free[cursor].has_value()) {
        auto& old = free[cursor];
        if (old) {
            retainedBytes -= old->allocationBytes;
            destroy(*old);
            old.reset();
        }
        cursor = (cursor + 1) % free.size();
    }
    free[cursor] = allocation;
    retainedBytes += allocation.allocationBytes;
    cursor = (cursor + 1) % free.size();
}

std::shared_ptr<BufferPool> GetBufferPool(const Context& context) {
    if (!context.bufferPool) context.bufferPool = std::make_shared<BufferPool>(context);
    return context.bufferPool;
}

}
