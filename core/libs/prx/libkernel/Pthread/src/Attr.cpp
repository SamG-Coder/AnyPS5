#include "../include/Pthread.hpp"
#include "prx/libc/include/General.hpp"
#include <cstddef>
#include <new>

static constexpr int SCE_OK = 0;
static constexpr int SCE_KERNEL_ERROR_ENOMEM = 0x8002000C;
static constexpr int SCE_KERNEL_ERROR_EINVAL = 0x80020016;
static constexpr int SCE_KERNEL_ERROR_ENOTSUP = 0x8002002D;

static constexpr std::size_t DEFAULT_STACK_SIZE = 1u << 20;
static constexpr std::size_t MINIMUM_STACK = 512u * 4u;
static constexpr int DETACH_JOINABLE = 0;
static constexpr int DETACH_DETACHED = 1;
static constexpr int SCHED_FIFO_PS5 = 1;
static constexpr int SCHED_OTHER_PS5 = 2;
static constexpr int SCHED_RR_PS5 = 3;
static constexpr int INHERIT_SCHED = 4;
static constexpr int EXPLICIT_SCHED = 0;
static constexpr int DEFAULT_PRIORITY = 700;

static bool Live(const PthreadAttr* attr) { return attr && *attr; }

extern "C" {

int APS5_VABI scePthreadAttrInit(PthreadAttr* attr) {
    if (!attr) return SCE_KERNEL_ERROR_EINVAL;
    auto* created = new (std::nothrow) PthreadAttrPrivate{};
    if (!created) return SCE_KERNEL_ERROR_ENOMEM;
    created->_stacksize = DEFAULT_STACK_SIZE;
    created->_guardsize = 0;
    created->_detachstate = DETACH_JOINABLE;
    created->_schedpriority = DEFAULT_PRIORITY;
    created->_schedpolicy = SCHED_FIFO_PS5;
    created->_inheritsched = INHERIT_SCHED;
    *attr = created;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrDestroy(PthreadAttr* attr) {
    if (!Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    delete *attr;
    *attr = nullptr;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetdetachstate(PthreadAttr* attr, int detachstate) {
    if (!Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    if (detachstate != DETACH_JOINABLE && detachstate != DETACH_DETACHED) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_detachstate = detachstate;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetdetachstate(const PthreadAttr* attr, int* state) {
    if (!Live(attr) || !state) return SCE_KERNEL_ERROR_EINVAL;
    *state = (*attr)->_detachstate;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetschedparam(PthreadAttr* attr, const KernelSchedParam* param) {
    if (!Live(attr) || !param) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_schedpriority = param->sched_priority;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetschedparam(const PthreadAttr* attr, KernelSchedParam* param) {
    if (!Live(attr) || !param) return SCE_KERNEL_ERROR_EINVAL;
    param->sched_priority = (*attr)->_schedpriority;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetschedpolicy(PthreadAttr* attr, int policy) {
    if (!Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    if (policy != SCHED_FIFO_PS5 && policy != SCHED_OTHER_PS5 && policy != SCHED_RR_PS5)
        return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_schedpolicy = policy;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetschedpolicy(const PthreadAttr* attr, int* policy) {
    if (!Live(attr) || !policy) return SCE_KERNEL_ERROR_EINVAL;
    *policy = (*attr)->_schedpolicy;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetinheritsched(PthreadAttr* attr, int inherit) {
    if (!Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    if (inherit != INHERIT_SCHED && inherit != EXPLICIT_SCHED) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_inheritsched = inherit;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetinheritsched(const PthreadAttr* attr, int* inherit) {
    if (!Live(attr) || !inherit) return SCE_KERNEL_ERROR_EINVAL;
    *inherit = (*attr)->_inheritsched;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetstacksize(PthreadAttr* attr, std::size_t stacksize) {
    if (!Live(attr) || stacksize < MINIMUM_STACK) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_stacksize = stacksize;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetstacksize(const PthreadAttr* attr, std::size_t* stacksize) {
    if (!Live(attr) || !stacksize) return SCE_KERNEL_ERROR_EINVAL;
    *stacksize = (*attr)->_stacksize;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetguardsize(PthreadAttr* attr, std::size_t guard) {
    if (!Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    if (guard != 0) return SCE_KERNEL_ERROR_ENOTSUP;
    (*attr)->_guardsize = 0;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetguardsize(const PthreadAttr* attr, std::size_t* guard) {
    if (!Live(attr) || !guard) return SCE_KERNEL_ERROR_EINVAL;
    *guard = (*attr)->_guardsize;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetstack(PthreadAttr* attr, void* address, std::size_t stacksize) {
    if (!Live(attr) || !address || stacksize < MINIMUM_STACK) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->stackAddress = address;
    (*attr)->_stacksize = stacksize;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrSetstackaddr(PthreadAttr* attr, void* address) {
    if (!Live(attr) || !address) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->stackAddress = address;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetstackaddr(const PthreadAttr* attr, void** address) {
    if (!Live(attr) || !address) return SCE_KERNEL_ERROR_EINVAL;
    *address = (*attr)->stackAddress;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetstack(const PthreadAttr* attr, void** address, std::size_t* stacksize) {
    if (!Live(attr) || !address || !stacksize) return SCE_KERNEL_ERROR_EINVAL;
    *address = (*attr)->stackAddress;
    *stacksize = (*attr)->_stacksize;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGet(Pthread thread, PthreadAttr* attr) {
    if (!thread || !Live(attr)) return SCE_KERNEL_ERROR_EINVAL;
    (*attr)->_stacksize = thread->stackSize;
    (*attr)->stackAddress = thread->stackAddress;
    (*attr)->_guardsize = thread->guardSize;
    (*attr)->_detachstate = thread->_detached ? DETACH_DETACHED : DETACH_JOINABLE;
    (*attr)->_schedpriority = DEFAULT_PRIORITY;
    (*attr)->_schedpolicy = SCHED_FIFO_PS5;
    (*attr)->_inheritsched = INHERIT_SCHED;
    return SCE_OK;
}

int APS5_VABI scePthreadAttrGetaffinity(const PthreadAttr* attr, KernelCpumask* mask) {
    (void)attr;
    (void)mask;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI scePthreadAttrGetsolosched(const PthreadAttr* attr, int* solosched) {
    (void)attr;
    (void)solosched;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI scePthreadAttrSetaffinity(PthreadAttr* attr, KernelCpumask mask) {
    (void)attr;
    (void)mask;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI scePthreadAttrSetsolosched(PthreadAttr* attr, int solosched) {
    (void)attr;
    (void)solosched;
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
