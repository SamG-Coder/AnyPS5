#include "../include/Pthread.hpp"
#include "prx/libc/include/General.hpp"

// ---------------------------------------------------------------------------
// POSIX-style pthread semaphores (scePthreadSem*). Not to be confused with
// the native sceKernel* semaphore in libkernel/Semaphore. Moved as-is
// (not yet implemented) from the duplicated stubs in libkernel_sys/Export.cpp
// and libkernel_web/Export.cpp.
// ---------------------------------------------------------------------------

extern "C" {

int APS5_VABI scePthreadSemDestroy(void* sem) {
 (void)sem;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemGetvalue(void* sem, int* value) {
 (void)sem;
 (void)value;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemInit(void* sem, int flag, unsigned int value, const char* name) {
 (void)sem;
 (void)flag;
 (void)value;
 (void)name;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemPost(void* sem) {
 (void)sem;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemTimedwait(void* sem, KernelUseconds usec) {
 (void)sem;
 (void)usec;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemTrywait(void* sem) {
 (void)sem;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadSemWait(void* sem) {
 (void)sem;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

}
