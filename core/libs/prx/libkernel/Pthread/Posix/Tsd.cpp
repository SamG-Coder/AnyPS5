#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"

extern "C" {
int APS5_VABI scePthreadKeyCreate(PthreadKey*, pthread_key_destructor_func_t);
int APS5_VABI scePthreadKeyDelete(PthreadKey);
void* APS5_VABI scePthreadGetspecific(PthreadKey);
int APS5_VABI scePthreadSetspecific(PthreadKey, void*);

static int GuestKeyResult(int result) { return result == 0 ? 0 : static_cast<unsigned>(result) & 0xffff; }

void* APS5_VABI pthread_getspecific_nid_postfix(PthreadKey key) {
    return scePthreadGetspecific(key);
}

int APS5_VABI pthread_setspecific_nid_postfix(PthreadKey key, void* value) {
    return GuestKeyResult(scePthreadSetspecific(key, value));
}

int APS5_VABI pthread_key_create_nid_postfix(PthreadKey* key, pthread_key_destructor_func_t destructor) {
    return GuestKeyResult(scePthreadKeyCreate(key, destructor));
}

int APS5_VABI pthread_key_delete_nid_postfix(PthreadKey key) {
    return GuestKeyResult(scePthreadKeyDelete(key));
}

}
