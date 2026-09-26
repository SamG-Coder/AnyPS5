#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include <mutex>
#include <stdexcept>
#include <unordered_map>
namespace {
struct NativeCommandBufferState {
    std::uint64_t indexBuffer = 0;
    std::uint32_t indexCount = 0;
    std::uint8_t indexSize = 0;
    std::uint32_t instances = 1;
};
std::mutex stateMutex;
std::unordered_map<CommandBuffer*, NativeCommandBufferState> states;
NativeCommandBufferState& state(CommandBuffer* buffer) {
    if (!buffer) throw std::invalid_argument("native AGC: null command buffer");
    return states[buffer];
}
std::uint32_t* opaque(CommandBuffer* buffer) {
    // The lowered ABI still returns a command handle because original code may
    // retain the value. It is not executable PS5 PM4 and is never interpreted.
    return buffer ? buffer->cursor_up : nullptr;
}
}
extern "C" {
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexBuffer(CommandBuffer* b, std::uint64_t address) {
    if (!address) throw std::invalid_argument("native AGC: null index buffer");
    std::lock_guard lock(stateMutex); state(b).indexBuffer = address; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexCount(CommandBuffer* b, std::uint32_t count) {
    std::lock_guard lock(stateMutex); state(b).indexCount = count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexSize(CommandBuffer* b, std::uint8_t size, std::uint8_t) {
    if (size > 2) throw std::invalid_argument("native AGC: invalid index size");
    std::lock_guard lock(stateMutex); state(b).indexSize = size; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcSetNumInstances(CommandBuffer* b, std::uint32_t count) {
    std::lock_guard lock(stateMutex); state(b).instances = count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndex(CommandBuffer* b, std::uint32_t count, const volatile void* address, std::uint64_t) {
    if (!address) throw std::invalid_argument("native AGC: null draw index address");
    std::lock_guard lock(stateMutex); auto& s=state(b); s.indexBuffer=reinterpret_cast<std::uintptr_t>(address); s.indexCount=count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexAuto(CommandBuffer* b, std::uint32_t count, std::uint64_t) {
    std::lock_guard lock(stateMutex); state(b).indexCount=count; return opaque(b);
}
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexOffset(CommandBuffer* b, std::uint32_t, std::uint32_t count, std::uint64_t) {
    std::lock_guard lock(stateMutex); state(b).indexCount=count; return opaque(b);
}
int APS5_VABI aps5NativeAgcSubmit(const Packet*) {
    throw std::runtime_error("native AGC submission reached before native Vulkan lowering is complete");
}
}
