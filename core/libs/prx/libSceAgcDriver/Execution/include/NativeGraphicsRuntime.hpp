#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEGRAPHICSRUNTIME_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVEGRAPHICSRUNTIME_HPP
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include <memory>
#include <mutex>
namespace AgcDriver {
class NativeGraphicsRuntime {
public:
    static NativeGraphicsRuntime& Get();
    VulkanDevice& Headless();
    VulkanDevice& Presenting(const PresentationWindow& window);
    void ReleaseWindow(void* window);
    std::recursive_mutex& Mutex(){return mutex;}
private:
    std::recursive_mutex mutex;
    std::shared_ptr<VulkanDevice> device;
};
}
#endif
