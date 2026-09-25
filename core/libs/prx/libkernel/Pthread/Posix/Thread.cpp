#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include <new>
#include <system_error>


extern "C" {
int APS5_VABI scePthreadCreate(Pthread*, const PthreadAttr*, PthreadEntry, void*, const char*);
int APS5_VABI scePthreadJoin(Pthread, void**);
Pthread APS5_VABI scePthreadSelf();
int APS5_VABI scePthreadEqual(Pthread, Pthread);
void APS5_VABI scePthreadYield();
int APS5_VABI scePthreadRename(Pthread, const char*);
int APS5_VABI scePthreadSetcancelstate(int, int*);
int APS5_VABI scePthreadSetcanceltype(int, int*);

void APS5_VABI pthread_set_name_np_nid_postfix(Pthread thread, const char* name) {
    if (scePthreadRename(thread, name) != 0)
        throw std::runtime_error("pthread_set_name_np: cannot name guest thread");
}

int APS5_VABI pthread_create_nid_postfix(Pthread* thread, const PthreadAttr* attr, pthread_entry_func_t entry, void* arg) {
    if (!thread || !entry || (attr && !*attr)) return 22;
    try {
        const auto result = scePthreadCreate(thread, attr, entry, arg, nullptr);
        return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff;
    } catch (const std::bad_alloc&) { return 12; }
      catch (const std::system_error& error) {
        if (error.code() == std::errc::resource_unavailable_try_again) return 35;
        if (error.code() == std::errc::not_enough_memory) return 12;
        throw;
    }
}

int APS5_VABI pthread_create_name_np_nid_postfix(Pthread* thread, const PthreadAttr* attr, pthread_entry_func_t entry, void* arg, const char* name) {
 (void)thread;
 (void)attr;
 (void)entry;
 (void)arg;
 (void)name;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI pthread_detach_nid_postfix(Pthread thread) {
 (void)thread;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

void APS5_VABI pthread_exit_nid_postfix(void* value) {
 (void)value;
 NotImplemented_nid_no_patch(__func__);
}

int APS5_VABI pthread_getschedparam_nid_postfix(Pthread thread, int* policy, KernelSchedParam* param) {
 (void)thread;
 (void)policy;
 (void)param;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI pthread_join_nid_postfix(Pthread thread, void** value) {
    if (!thread) return 22;
    if (thread == scePthreadSelf()) return 11;
    const auto result = scePthreadJoin(thread, value);
    return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff;
}

int APS5_VABI pthread_rename_np_nid_postfix(Pthread thread, const char* name) {
    const int result = scePthreadRename(thread, name);
    return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff;
}

Pthread APS5_VABI pthread_self_nid_postfix(void) {
    return scePthreadSelf();
}

int APS5_VABI pthread_equal_nid_postfix(Pthread first, Pthread second) {
    return scePthreadEqual(first, second);
}

int APS5_VABI pthread_setcancelstate_nid_postfix(int state, int* old_state) {
    const int result = scePthreadSetcancelstate(state, old_state);
    return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff;
}

int APS5_VABI pthread_setcanceltype_nid_postfix(int type, int* old_type) {
    const int result = scePthreadSetcanceltype(type, old_type);
    return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff;
}

int APS5_VABI pthread_setprio_nid_postfix(Pthread thread, int prio) {
 (void)thread;
 (void)prio;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI pthread_setschedparam_nid_postfix(Pthread thread, int policy, const KernelSchedParam* param) {
 (void)thread;
 (void)policy;
 (void)param;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

void APS5_VABI pthread_yield_nid_postfix(void) {
    scePthreadYield();
}

int APS5_VABI sched_yield_nid_postfix() {
    scePthreadYield();
    return 0;
}

}
