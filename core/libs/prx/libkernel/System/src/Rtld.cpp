#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

extern "C" {

void APS5_VABI sceKernelRtldSetApplicationHeapAPI(void* api[]) {
 (void)api;
 NotImplemented_nid_no_patch(__func__);
}

int APS5_VABI sceKernelRtldThreadAtexitDecrement(uint64_t* c) {
 (void)c;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelRtldThreadAtexitIncrement(uint64_t* c) {
 (void)c;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

void APS5_VABI sceKernelSetThreadAtexitCount(get_thread_atexit_count_func_t func) {
 (void)func;
 NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI sceKernelSetThreadAtexitReport(thread_atexit_report_func_t func) {
 (void)func;
 NotImplemented_nid_no_patch(__func__);
}

void APS5_VABI sceKernelSetThreadDtors(thread_dtors_func_t dtors) {
 (void)dtors;
 NotImplemented_nid_no_patch(__func__);
}

}
