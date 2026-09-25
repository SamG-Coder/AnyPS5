#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"

extern "C" {
int APS5_VABI scePthreadAttrInit(PthreadAttr*);
int APS5_VABI scePthreadAttrDestroy(PthreadAttr*);
int APS5_VABI scePthreadAttrGet(Pthread, PthreadAttr*);
int APS5_VABI scePthreadAttrGetdetachstate(const PthreadAttr*, int*);
int APS5_VABI scePthreadAttrGetguardsize(const PthreadAttr*, std::size_t*);
int APS5_VABI scePthreadAttrGetinheritsched(const PthreadAttr*, int*);
int APS5_VABI scePthreadAttrGetschedparam(const PthreadAttr*, KernelSchedParam*);
int APS5_VABI scePthreadAttrGetschedpolicy(const PthreadAttr*, int*);
int APS5_VABI scePthreadAttrGetstack(const PthreadAttr*, void**, std::size_t*);
int APS5_VABI scePthreadAttrGetstacksize(const PthreadAttr*, std::size_t*);
int APS5_VABI scePthreadAttrSetdetachstate(PthreadAttr*, int);
int APS5_VABI scePthreadAttrSetguardsize(PthreadAttr*, std::size_t);
int APS5_VABI scePthreadAttrSetinheritsched(PthreadAttr*, int);
int APS5_VABI scePthreadAttrSetschedparam(PthreadAttr*, const KernelSchedParam*);
int APS5_VABI scePthreadAttrSetschedpolicy(PthreadAttr*, int);
int APS5_VABI scePthreadAttrSetstacksize(PthreadAttr*, std::size_t);

static int AttrResult(int result) { return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff; }

int APS5_VABI pthread_attr_destroy_nid_postfix(PthreadAttr* attr) {
    return AttrResult(scePthreadAttrDestroy(attr));
}

int APS5_VABI pthread_attr_get_np_nid_postfix(Pthread thread, PthreadAttr* attr) {
    return AttrResult(scePthreadAttrGet(thread, attr));
}

int APS5_VABI pthread_attr_getdetachstate_nid_postfix(const PthreadAttr* attr, int* state) {
    return AttrResult(scePthreadAttrGetdetachstate(attr, state));
}

int APS5_VABI pthread_attr_getguardsize_nid_postfix(const PthreadAttr* attr, std::size_t* guard) {
    return AttrResult(scePthreadAttrGetguardsize(attr, guard));
}

int APS5_VABI pthread_attr_getinheritsched_nid_postfix(const PthreadAttr* attr, int* inherit) {
    return AttrResult(scePthreadAttrGetinheritsched(attr, inherit));
}

int APS5_VABI pthread_attr_getschedparam_nid_postfix(const PthreadAttr* attr, KernelSchedParam* param) {
    return AttrResult(scePthreadAttrGetschedparam(attr, param));
}

int APS5_VABI pthread_attr_getschedpolicy_nid_postfix(const PthreadAttr* attr, int* policy) {
    return AttrResult(scePthreadAttrGetschedpolicy(attr, policy));
}

int APS5_VABI pthread_attr_getstack_nid_postfix(const PthreadAttr* attr, void** address, std::size_t* stack) {
    return AttrResult(scePthreadAttrGetstack(attr, address, stack));
}

int APS5_VABI pthread_attr_getstacksize_nid_postfix(const PthreadAttr* attr, std::size_t* stack) {
    return AttrResult(scePthreadAttrGetstacksize(attr, stack));
}

int APS5_VABI pthread_attr_init_nid_postfix(PthreadAttr* attr) {
    return AttrResult(scePthreadAttrInit(attr));
}

int APS5_VABI pthread_attr_setdetachstate_nid_postfix(PthreadAttr* attr, int state) {
    return AttrResult(scePthreadAttrSetdetachstate(attr, state));
}

int APS5_VABI pthread_attr_setguardsize_nid_postfix(PthreadAttr* attr, std::size_t guard) {
    return AttrResult(scePthreadAttrSetguardsize(attr, guard));
}

int APS5_VABI pthread_attr_setinheritsched_nid_postfix(PthreadAttr* attr, int inherit) {
    return AttrResult(scePthreadAttrSetinheritsched(attr, inherit));
}

int APS5_VABI pthread_attr_setschedparam_nid_postfix(PthreadAttr* attr, const KernelSchedParam* param) {
    return AttrResult(scePthreadAttrSetschedparam(attr, param));
}

int APS5_VABI pthread_attr_setschedpolicy_nid_postfix(PthreadAttr* attr, int policy) {
    return AttrResult(scePthreadAttrSetschedpolicy(attr, policy));
}

int APS5_VABI pthread_attr_setstacksize_nid_postfix(PthreadAttr* attr, std::size_t stack) {
    return AttrResult(scePthreadAttrSetstacksize(attr, stack));
}

}
