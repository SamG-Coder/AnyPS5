#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEAGC_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEAGC_HPP
#include "SceTypes.hpp"
#include "SceShaders.hpp"
#include <cstdint>
extern "C" {
std::uint32_t* APS5_VABI aps5NativeAgcSetCxRegistersIndirect(CommandBuffer*, const volatile ShaderRegister*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegistersIndirect(CommandBuffer*, const volatile ShaderRegister*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegistersIndirect(CommandBuffer*, const volatile ShaderRegister*, std::uint32_t);
int APS5_VABI aps5NativeAgcCreateShader(Shader**, void*, const volatile void*);
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisters(CommandBuffer*, const volatile ShaderRegister*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetShRegisterRange(CommandBuffer*, std::uint32_t, const std::uint32_t*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisters(CommandBuffer*, const volatile ShaderRegister*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetUcRegisterRange(CommandBuffer*, std::uint32_t, const std::uint32_t*, std::uint32_t);

std::uint32_t* APS5_VABI aps5NativeAgcDrawIndex(CommandBuffer*, std::uint32_t, const volatile void*, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexAuto(CommandBuffer*, std::uint32_t, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcDrawIndexOffset(CommandBuffer*, std::uint32_t, std::uint32_t, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexBuffer(CommandBuffer*, std::uint64_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexCount(CommandBuffer*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetIndexSize(CommandBuffer*, std::uint8_t, std::uint8_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetNumInstances(CommandBuffer*, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcReleaseMem(CommandBuffer*, std::uint8_t, std::uint16_t,
    std::uint8_t, std::uint8_t, const volatile Label*, std::uint8_t, std::uint64_t,
    std::uint16_t, std::uint16_t, std::uint8_t, std::uint32_t);
std::uint32_t* APS5_VABI aps5NativeAgcSetFlip(CommandBuffer*, std::uint32_t, std::int32_t, std::uint32_t, std::int64_t);
std::uint32_t APS5_VABI aps5NativeAgcGetWaitRenderingSize();
std::uint32_t APS5_VABI aps5NativeAgcWaitUntilSafeForRendering(std::uint32_t**, std::uint32_t, std::uint32_t, std::uint32_t, int);
int APS5_VABI aps5NativeAgcSubmit(const Packet*);
}
#endif
