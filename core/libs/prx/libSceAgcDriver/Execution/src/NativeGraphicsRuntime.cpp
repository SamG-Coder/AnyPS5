#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include <stdexcept>

namespace AgcDriver {
NativeGraphicsRuntime::NativeGraphicsRuntime() {
    LibcRegisterShutdown_nid_postfix([] { Get().Shutdown(); });
}

void NativeGraphicsRuntime::Shutdown() {
    completions.Stop();
    std::lock_guard lock(mutex);
    device.reset();
}

NativeGraphicsRuntime& NativeGraphicsRuntime::Get() {
    static NativeGraphicsRuntime runtime;
    return runtime;
}

void NativeGraphicsRuntime::CheckFailure() const {
    std::exception_ptr error;
    {
        std::lock_guard lock(failureMutex);
        error = failure;
    }
    if (error) std::rethrow_exception(error);
}

bool NativeGraphicsRuntime::ReportFailure(std::exception_ptr error) noexcept {
    if (!error) std::terminate();
    std::lock_guard lock(failureMutex);
    if (failure) return false;
    failure = error;
    return true;
}

VulkanDevice& NativeGraphicsRuntime::Headless() {
    std::lock_guard lock(mutex);
    CheckFailure();
    if (!device) device = std::make_shared<VulkanDevice>();
    return *device;
}

VulkanDevice& NativeGraphicsRuntime::Presenting(const PresentationWindow& window) {
    std::lock_guard lock(mutex);
    CheckFailure();
    if (!device || device->Window() == nullptr) {
        if (device) device->WaitIdle();
        device = std::make_shared<VulkanDevice>(&window);
    }
    if (device->Window() != window.context)
        throw std::runtime_error("native graphics: presentation window does not match Vulkan surface");
    return *device;
}

void NativeGraphicsRuntime::Present(const PresentationWindow& window, const DisplayBuffer* buffer,
                                    bool opaque, void (*gpuReady)(void*), void* context) {
    CheckFailure();
    if (!gpuReady || !context || !window.getDrawableSize)
        throw std::invalid_argument("native graphics: invalid presentation request");
    {
        std::lock_guard lock(mutex);
        auto& presenting = Presenting(window);
        std::uint32_t width = 0, height = 0;
        window.getDrawableSize(window.context, &width, &height);
        presenting.Resize(width, height);
        if (presenting.Presentable()) {
            if (buffer) {
                if (buffer->width != window.width || buffer->height != window.height)
                    throw std::runtime_error("native graphics: display buffer extent differs from output");
                presenting.WaitDraws();
                presenting.PresentDisplayBuffer(*buffer);
            } else {
                presenting.PresentClear(window.width, window.height, opaque);
            }
        }
    }
    // VideoOut callbacks acquire their own locks and may re-enter renderer
    // services. Never call them with the native GPU mutex held.
    CheckFailure();
    gpuReady(context);
}

void NativeGraphicsRuntime::WaitDraws() {
    std::exception_ptr error;
    {
        std::lock_guard lock(mutex);
        try {
            CheckFailure();
            if (device) device->WaitDraws();
        } catch (...) {
            error = std::current_exception();
        }
    }
    if (error) AgcDriverReportFailure_nid_postfix(error);
    completions.WaitIdle();
    if (error) std::rethrow_exception(error);
    CheckFailure();
}

void NativeGraphicsRuntime::CompleteAfterDraws(std::function<void()> complete, std::function<void(std::exception_ptr)> fail) {
    std::lock_guard lock(mutex);
    CheckFailure();
    const auto submittedDevice = device;
    const auto serial = submittedDevice ? submittedDevice->SubmitDraws() : 0;
    completions.Enqueue(
        [this, submittedDevice, serial] {
            std::lock_guard gpuLock(mutex);
            CheckFailure();
            return !submittedDevice || submittedDevice->CompletedDraws() >= serial;
        }, std::move(complete),
        [this, fail = std::move(fail)](std::exception_ptr error) {
            AgcDriverReportFailure_nid_postfix(error);
            ReportFailure(error);
            fail(error);
        });
}

void NativeGraphicsRuntime::ReleaseWindow(void* window) {
    std::lock_guard lock(mutex);
    // Cleanup is still permitted after the first asynchronous failure.
    if (device && device->Window() == window) device.reset();
}
}
