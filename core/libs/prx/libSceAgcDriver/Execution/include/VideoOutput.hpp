#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VIDEOOUTPUT_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_VIDEOOUTPUT_HPP

#include <cstdint>
#include <exception>
#include <memory>

namespace AgcDriver {

class FrameTiming;

inline constexpr std::uint32_t FlipPacketHeader = 0xc004105cu;
inline constexpr std::uint32_t FlipPacketWords = 6;

struct FlipInfo {
    std::uint32_t handle;
    std::int32_t index;
    std::uint32_t mode;
    std::int64_t argument;
};

class IFlipRequest {
public:
    virtual ~IFlipRequest() = default;
    virtual void GpuReady(const std::shared_ptr<FrameTiming>& timing) = 0;
    virtual void Fail(std::exception_ptr error) noexcept = 0;
};

class IVideoOutput {
public:
    virtual ~IVideoOutput() = default;
    virtual std::shared_ptr<IFlipRequest> Reserve(const FlipInfo& info) = 0;
    virtual void Fail(std::exception_ptr error) noexcept = 0;
};

}

extern "C" void AgcDriverRegisterVideoOutput_nid_postfix(std::uint32_t handle, const std::shared_ptr<AgcDriver::IVideoOutput>& output);
extern "C" void AgcDriverUnregisterVideoOutput_nid_postfix(std::uint32_t handle, const std::shared_ptr<AgcDriver::IVideoOutput>& output);

#endif
