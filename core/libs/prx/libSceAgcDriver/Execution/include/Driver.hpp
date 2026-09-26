#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_DRIVER_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_DRIVER_HPP

#include "SceTypes.hpp"
#include "SceShaders.hpp"

namespace AgcDriver {

void Submit(const Packet* packet, std::uint32_t queue);

struct NativeDrawHint {
    const std::uint32_t* packet;
    std::uint32_t words;
    std::uint64_t indexAddress;
    std::uint32_t indexCount;
    std::uint32_t indexSize;
    std::uint32_t flags;
    bool indexed;
    std::uint32_t indexOffset;
};

void RegisterNativeDrawHint(const NativeDrawHint& hint);

}

extern "C" void AgcDriverWaitIdle_nid_postfix();
extern "C" void AgcDriverSuspendPoint_nid_postfix();
extern "C" void AgcDriverRegisterShader_nid_postfix(const Shader* shader);

#endif
