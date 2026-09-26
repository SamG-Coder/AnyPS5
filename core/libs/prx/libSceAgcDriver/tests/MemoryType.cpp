#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"

int main() {
    using namespace AgcDriver::Graphics;
    Context context{};
    constexpr auto required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    constexpr auto cached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
    context.memory.memoryTypeCount = 4;
    context.memory.memoryTypes[0].propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    context.memory.memoryTypes[1].propertyFlags = required;
    context.memory.memoryTypes[2].propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | cached;
    context.memory.memoryTypes[3].propertyFlags = required | cached;
    Require(context.MemoryType(15, required) == 1, "default memory selection changed");
    Require(context.MemoryType(15, required, cached) == 3, "cached coherent memory was not preferred");
    Require(context.MemoryType(7, required, cached) == 1, "fallback dropped the coherence requirement");
    Require(context.MemoryType(2, required, cached) == 1, "uncached-only device fallback failed");
    Require(context.MemoryType(15, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, cached) == 0, "device-local requirement was lost");
    for (const auto mask : {0u, 1u, 4u}) {
        bool rejected = false;
        try { context.MemoryType(mask, required, cached); }
        catch (const std::runtime_error&) { rejected = true; }
        Require(rejected, "incompatible memory type accepted");
    }
}
