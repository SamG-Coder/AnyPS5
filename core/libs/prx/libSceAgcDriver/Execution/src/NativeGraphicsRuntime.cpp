#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include <stdexcept>
namespace AgcDriver {
NativeGraphicsRuntime& NativeGraphicsRuntime::Get(){static NativeGraphicsRuntime runtime;return runtime;}
VulkanDevice& NativeGraphicsRuntime::Headless(){
    std::lock_guard lock(mutex);
    if(!device) device=std::make_shared<VulkanDevice>();
    return *device;
}
VulkanDevice& NativeGraphicsRuntime::Presenting(const PresentationWindow& window){
    std::lock_guard lock(mutex);
    if(!device||device->Window()==nullptr){if(device)device->WaitIdle();device=std::make_shared<VulkanDevice>(&window);}
    if(device->Window()!=window.context) throw std::runtime_error("native graphics: presentation window does not match Vulkan surface");
    return *device;
}
void NativeGraphicsRuntime::WaitDraws(){std::lock_guard lock(mutex);if(device)device->WaitDraws();}
void NativeGraphicsRuntime::ReleaseWindow(void* window){
    std::lock_guard lock(mutex);
    if(device&&device->Window()==window)device.reset();
}
}
