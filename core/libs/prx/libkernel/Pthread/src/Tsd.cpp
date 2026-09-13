#include "../include/Pthread.hpp"
#include "prx/libc/include/General.hpp"

// ---------------------------------------------------------------------------
// Thread-specific data (pthread keys). Moved as-is (not yet implemented)
// from the duplicated stubs in libkernel_sys/Export.cpp and
// libkernel_web/Export.cpp.
// ---------------------------------------------------------------------------

extern "C" {

int APS5_VABI scePthreadKeyCreate(PthreadKey* key, pthread_key_destructor_func_t destructor) {
 (void)key;
 (void)destructor;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI scePthreadKeyDelete(PthreadKey key) {
 (void)key;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

void* APS5_VABI scePthreadGetspecific(PthreadKey key) {
 (void)key;
 NotImplemented_nid_no_patch(__func__);
 return nullptr;
}

int APS5_VABI scePthreadSetspecific(PthreadKey key, void* value) {
 (void)key;
 (void)value;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

}
