#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <stdexcept>

namespace {
using Outputs = std::unordered_map<std::uint32_t, std::shared_ptr<AgcDriver::IVideoOutput>>;
// Registration is rare. Immutable snapshots let failure notification retain
// every output without allocating or calling user code under the registry lock.
std::shared_ptr<const Outputs> outputs = std::make_shared<const Outputs>();
std::mutex outputsMutex;
}

namespace AgcDriver {
std::shared_ptr<IRenderingWait> CaptureNativeRenderingWait(std::uint32_t handle, std::uint32_t index) {
    std::shared_ptr<IVideoOutput> output;
    {
        std::lock_guard lock(outputsMutex);
        NativeGraphicsRuntime::Get().CheckFailure();
        const auto found = outputs->find(handle);
        if (found == outputs->end()) throw std::invalid_argument("native graphics: unregistered rendering wait output");
        output = found->second;
    }
    auto wait = output->CaptureRenderingWait(index);
    if (!wait) throw std::runtime_error("native graphics: null rendering wait");
    return wait;
}
void Submit(const Packet* packet, std::uint32_t queue) {
    NativeGraphicsRuntime::Get().CheckFailure();
    if (queue != 0) throw std::runtime_error("native graphics: compute submission has no native lowering");
    aps5NativeAgcSubmit(packet);
}
}

extern "C" {
void AgcDriverSuspendPoint_nid_postfix() { AgcDriver::NativeGraphicsRuntime::Get().WaitDraws(); }
int APS5_VABI sceAgcDriverAgrSubmitDcb(const Packet* packet) { return aps5NativeAgcSubmit(packet); }
int APS5_VABI sceAgcDriverSubmitDcb(const Packet* packet) { return aps5NativeAgcSubmit(packet); }
void AgcDriverWaitIdle_nid_postfix() { AgcDriver::NativeGraphicsRuntime::Get().WaitDraws(); }

void AgcDriverRegisterVideoOutput_nid_postfix(std::uint32_t handle, const std::shared_ptr<AgcDriver::IVideoOutput>& output) {
    std::lock_guard lock(outputsMutex);
    AgcDriver::NativeGraphicsRuntime::Get().CheckFailure();
    if (!output || outputs->contains(handle))
        throw std::runtime_error("native graphics: invalid video output registration");
    auto updated = std::make_shared<Outputs>(*outputs);
    updated->emplace(handle, output);
    outputs = std::move(updated);
}

void AgcDriverUnregisterVideoOutput_nid_postfix(std::uint32_t handle, const std::shared_ptr<AgcDriver::IVideoOutput>& output) {
    std::lock_guard lock(outputsMutex);
    const auto it = outputs->find(handle);
    if (it == outputs->end() || it->second != output)
        throw std::runtime_error("native graphics: video output registration mismatch");
    auto updated = std::make_shared<Outputs>(*outputs);
    updated->erase(handle);
    outputs = std::move(updated);
}

void AgcDriverPresentClear_nid_postfix(const AgcDriver::PresentationWindow& window, bool opaque, void (*ready)(void*), void* context) {
    AgcDriver::NativeGraphicsRuntime::Get().Present(window, nullptr, opaque, ready, context);
}
void AgcDriverPresentBuffer_nid_postfix(const AgcDriver::PresentationWindow& window, const AgcDriver::DisplayBuffer& buffer, void (*ready)(void*), void* context) {
    AgcDriver::NativeGraphicsRuntime::Get().Present(window, &buffer, true, ready, context);
}
void AgcDriverReleaseWindow_nid_postfix(void* window) { AgcDriver::NativeGraphicsRuntime::Get().ReleaseWindow(window); }

void AgcDriverReportFailure_nid_postfix(std::exception_ptr error) {
    if (!AgcDriver::NativeGraphicsRuntime::Get().ReportFailure(error)) return;
    std::shared_ptr<const Outputs> notify;
    {
        std::lock_guard lock(outputsMutex);
        notify = outputs;
    }
    for (const auto& [handle, output] : *notify) output->Fail(error);
}
}
