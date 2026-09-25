#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libkernel/Pthread/include/Pthread.hpp"
#include <new>

extern "C" {

int APS5_VABI pthread_cond_broadcast_nid_postfix(PthreadCond* cond) {
    if (!cond) return 22;
    if (*cond) (*cond)->_cv.notify_all();
    return 0;
}

int APS5_VABI pthread_cond_init_nid_postfix(PthreadCond* cond, const PthreadCondattr* attr) {
    if (!cond || (attr && !*attr)) return 22;
    if (attr && (*attr)->_clockid != 0)
        throw std::runtime_error("pthread_cond_init: clock is not supported");
    auto* value = new (std::nothrow) PthreadCondPrivate{};
    if (!value) return 12;
    *cond = value;
    return 0;
}

int APS5_VABI pthread_cond_destroy_nid_postfix(PthreadCond* cond) {
    if (!cond) return 22;
    delete *cond;
    *cond = nullptr;
    return 0;
}

int APS5_VABI pthread_cond_signal_nid_postfix(PthreadCond* cond) {
    if (!cond) return 22;
    if (*cond) (*cond)->_cv.notify_one();
    return 0;
}

int APS5_VABI pthread_cond_timedwait_nid_postfix(PthreadCond* cond, PthreadMutex* mutex, const KernelTimespec* abstime) {
 (void)cond;
 (void)mutex;
 (void)abstime;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI pthread_cond_wait_nid_postfix(PthreadCond* cond, PthreadMutex* mutex) {
 (void)cond;
 (void)mutex;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI pthread_condattr_destroy_nid_postfix(PthreadCondattr* attr) {
    if (!attr || !*attr) return 22;
    delete *attr;
    *attr = nullptr;
    return 0;
}

int APS5_VABI pthread_condattr_init_nid_postfix(PthreadCondattr* attr) {
    if (!attr) return 22;
    auto* value = new (std::nothrow) PthreadCondattrPrivate{0};
    if (!value) return 12;
    *attr = value;
    return 0;
}

int APS5_VABI pthread_condattr_setclock_nid_postfix(PthreadCondattr* attr, KernelClockid clock_id) {
    if (!attr || !*attr || (clock_id != 0 && clock_id != 4)) return 22;
    if (clock_id != 0) throw std::runtime_error("pthread_condattr_setclock: clock is not supported");
    (*attr)->_clockid = clock_id;
    return 0;
}

}
