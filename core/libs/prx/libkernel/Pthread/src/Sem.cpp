#include <cerrno>
#include "SceTypes.hpp"

extern "C" {
int APS5_VABI sem_init_nid_postfix(void*, int, unsigned int);
int APS5_VABI sem_destroy_nid_postfix(void*);
int APS5_VABI sem_wait_nid_postfix(void*);
int APS5_VABI sem_trywait_nid_postfix(void*);
int APS5_VABI sem_post_nid_postfix(void*);
int APS5_VABI sem_getvalue_nid_postfix(void*, int*);
int APS5_VABI sem_reltimedwait_np_nid_postfix(void*, std::uint32_t);

static int SemResult() {
    const int error = errno;
    return static_cast<int>(0x80020000u | static_cast<unsigned>(error));
}

int APS5_VABI scePthreadSemInit(void* sem, int flag, unsigned int value, const char*) {
    const int saved = errno;
    const int result = sem_init_nid_postfix(sem, flag, value);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemDestroy(void* sem) {
    const int saved = errno;
    const int result = sem_destroy_nid_postfix(sem);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemGetvalue(void* sem, int* value) {
    const int saved = errno;
    const int result = sem_getvalue_nid_postfix(sem, value);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemPost(void* sem) {
    const int saved = errno;
    const int result = sem_post_nid_postfix(sem);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemTrywait(void* sem) {
    const int saved = errno;
    const int result = sem_trywait_nid_postfix(sem);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemWait(void* sem) {
    const int saved = errno;
    const int result = sem_wait_nid_postfix(sem);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

int APS5_VABI scePthreadSemTimedwait(void* sem, KernelUseconds usec) {
    const int saved = errno;
    const int result = sem_reltimedwait_np_nid_postfix(sem, usec);
    const int error = SemResult();
    errno = saved;
    return result == 0 ? 0 : error;
}

}
