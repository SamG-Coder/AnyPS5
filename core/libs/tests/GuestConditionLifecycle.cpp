#include "SceTypes.hpp"
#include <cstdlib>
extern "C" {
int APS5_VABI pthread_cond_init_nid_postfix(PthreadCond*, const PthreadCondattr*);
int APS5_VABI pthread_cond_destroy_nid_postfix(PthreadCond*);
int APS5_VABI pthread_cond_signal_nid_postfix(PthreadCond*);
int APS5_VABI pthread_cond_broadcast_nid_postfix(PthreadCond*);
int APS5_VABI pthread_condattr_init_nid_postfix(PthreadCondattr*);
int APS5_VABI pthread_condattr_destroy_nid_postfix(PthreadCondattr*);
int APS5_VABI pthread_condattr_setclock_nid_postfix(PthreadCondattr*, KernelClockid);
int APS5_VABI scePthreadCondInit(PthreadCond*, const PthreadCondattr*, const char*);
int APS5_VABI scePthreadCondDestroy(PthreadCond*);
int APS5_VABI scePthreadCondattrDestroy(PthreadCondattr*);
int APS5_VABI scePthreadCondattrInit(PthreadCondattr*);
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    PthreadCond condition = nullptr;
    Require(pthread_cond_signal_nid_postfix(&condition) == 0);
    Require(pthread_cond_broadcast_nid_postfix(&condition) == 0);
    Require(pthread_cond_destroy_nid_postfix(&condition) == 0);
    PthreadCondattr attribute = nullptr;
    Require(pthread_condattr_init_nid_postfix(&attribute) == 0 && attribute);
    Require(pthread_condattr_setclock_nid_postfix(&attribute, 0) == 0);
    Require(pthread_condattr_setclock_nid_postfix(&attribute, 12345) == 22);
    Require(pthread_cond_init_nid_postfix(&condition, &attribute) == 0 && condition);
    Require(pthread_cond_signal_nid_postfix(&condition) == 0);
    Require(pthread_cond_broadcast_nid_postfix(&condition) == 0);
    Require(scePthreadCondDestroy(&condition) == 0 && !condition);
    Require(scePthreadCondattrDestroy(&attribute) == 0 && !attribute);
    Require(scePthreadCondattrInit(&attribute) == 0);
    Require(scePthreadCondInit(&condition, &attribute, nullptr) == 0);
    Require(pthread_condattr_destroy_nid_postfix(&attribute) == 0 && !attribute);
    Require(pthread_cond_destroy_nid_postfix(&condition) == 0 && !condition);
    Require(pthread_cond_init_nid_postfix(nullptr, nullptr) == 22);
    Require(pthread_cond_init_nid_postfix(&condition, &attribute) == 22 && !condition);
    Require(pthread_cond_destroy_nid_postfix(nullptr) == 22);
    Require(pthread_cond_signal_nid_postfix(nullptr) == 22);
    Require(pthread_cond_broadcast_nid_postfix(nullptr) == 22);
    Require(pthread_condattr_init_nid_postfix(nullptr) == 22);
    Require(pthread_condattr_destroy_nid_postfix(&attribute) == 22);
    Require(pthread_condattr_setclock_nid_postfix(&attribute, 0) == 22);
}
