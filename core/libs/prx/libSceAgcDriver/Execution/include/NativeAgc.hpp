#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEAGC_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEAGC_HPP
#include "SceTypes.hpp"
#include <cstdint>
extern "C" {
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndex(CommandBuffer*, std::uint32_t, const volatile void*, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexAuto(CommandBuffer*, std::uint32_t, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexOffset(CommandBuffer*, std::uint32_t, std::uint32_t, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexBuffer(CommandBuffer*, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexCount(CommandBuffer*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexSize(CommandBuffer*, std::uint8_t, std::uint8_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetNumInstances(CommandBuffer*, std::uint32_t);
int APS5_VABI aps5NativeAgcSubmit(const Packet*);
}
#endif
