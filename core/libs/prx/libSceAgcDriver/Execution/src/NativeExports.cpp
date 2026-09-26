#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <stdexcept>
namespace {
std::unordered_map<std::uint32_t,std::shared_ptr<AgcDriver::IVideoOutput>> outputs;
std::mutex outputsMutex;
}
namespace AgcDriver {
void Submit(const Packet* packet, std::uint32_t queue) {
    if (queue != 0) throw std::runtime_error("native graphics: compute submission has no native lowering");
    aps5NativeAgcSubmit(packet);
}
}
extern "C" {
void AgcDriverSuspendPoint_nid_postfix(){AgcDriver::NativeGraphicsRuntime::Get().WaitDraws();}
int APS5_VABI sceAgcDriverAgrSubmitDcb(const Packet* packet){return aps5NativeAgcSubmit(packet);}
int APS5_VABI sceAgcDriverSubmitDcb(const Packet* packet){return aps5NativeAgcSubmit(packet);}
void AgcDriverWaitIdle_nid_postfix(){AgcDriver::NativeGraphicsRuntime::Get().WaitDraws();}
void AgcDriverRegisterVideoOutput_nid_postfix(std::uint32_t handle,const std::shared_ptr<AgcDriver::IVideoOutput>& output){std::lock_guard lock(outputsMutex);if(!output||!outputs.emplace(handle,output).second)throw std::runtime_error("native graphics: invalid video output registration");}
void AgcDriverUnregisterVideoOutput_nid_postfix(std::uint32_t handle,const std::shared_ptr<AgcDriver::IVideoOutput>& output){std::lock_guard lock(outputsMutex);auto it=outputs.find(handle);if(it==outputs.end()||it->second!=output)throw std::runtime_error("native graphics: video output registration mismatch");outputs.erase(it);}
void AgcDriverPresentClear_nid_postfix(const AgcDriver::PresentationWindow& window,bool opaque,void(*ready)(void*),void* context){AgcDriver::NativeGraphicsRuntime::Get().Present(window,nullptr,opaque,ready,context);}
void AgcDriverPresentBuffer_nid_postfix(const AgcDriver::PresentationWindow& window,const AgcDriver::DisplayBuffer& buffer,void(*ready)(void*),void* context){AgcDriver::NativeGraphicsRuntime::Get().Present(window,&buffer,true,ready,context);}
void AgcDriverReleaseWindow_nid_postfix(void* window){AgcDriver::NativeGraphicsRuntime::Get().ReleaseWindow(window);}
void AgcDriverReportFailure_nid_postfix(std::exception_ptr error){if(error)std::rethrow_exception(error);}
}
