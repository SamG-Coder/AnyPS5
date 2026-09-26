#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEGRAPHICSRUNTIME_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEGRAPHICSRUNTIME_HPP
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
#include "prx/libSceAgcDriver/Execution/include/DisplayBuffer.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeCompletionQueue.hpp"
#include <memory>
#include <mutex>
namespace AgcDriver {
class VulkanDevice;
class NativeGraphicsRuntime {
public:
    static NativeGraphicsRuntime& Get();
    VulkanDevice& Headless();
    VulkanDevice& Presenting(const PresentationWindow& window);
    void ReleaseWindow(void* window);
    void WaitDraws();
    void CompleteAfterDraws(std::function<void()> complete, std::function<void(std::exception_ptr)> fail);
    void CheckFailure() const;
    bool ReportFailure(std::exception_ptr error) noexcept;
    void Present(const PresentationWindow& window, const DisplayBuffer* buffer, bool opaque, void (*gpuReady)(void*), void* context);
    std::recursive_mutex& Mutex(){return mutex;}
private:
    NativeGraphicsRuntime();
    void Shutdown();
    mutable std::mutex failureMutex;
    std::exception_ptr failure;
    std::recursive_mutex mutex;
    std::shared_ptr<VulkanDevice> device;
    NativeCompletionQueue completions;
};
}
#endif
