#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
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
void NativeGraphicsRuntime::Present(const PresentationWindow& window,const DisplayBuffer* buffer,bool opaque,void(*gpuReady)(void*),void* context){
    if(!gpuReady||!context||!window.getDrawableSize) throw std::invalid_argument("native graphics: invalid presentation request");
    std::lock_guard lock(mutex);
    auto& presenting=Presenting(window);
    std::uint32_t width=0,height=0; window.getDrawableSize(window.context,&width,&height);
    presenting.Resize(width,height);
    if(presenting.Presentable()){
        if(buffer){if(buffer->width!=window.width||buffer->height!=window.height)throw std::runtime_error("native graphics: display buffer extent differs from output");presenting.WaitDraws();presenting.PresentDisplayBuffer(*buffer);}
        else presenting.PresentClear(window.width,window.height,opaque);
    }
    gpuReady(context);
}
void NativeGraphicsRuntime::WaitDraws(){std::lock_guard lock(mutex);if(device)device->WaitDraws();}
void NativeGraphicsRuntime::ReleaseWindow(void* window){
    std::lock_guard lock(mutex);
    if(device&&device->Window()==window)device.reset();
}
}
