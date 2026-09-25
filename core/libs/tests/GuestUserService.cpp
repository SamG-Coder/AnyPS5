#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libSceUserService/UserService.hpp"
#include "SceTypes.hpp"
#include <cstdlib>

extern "C" {
int APS5_VABI sceUserServiceInitialize(const void*);
int APS5_VABI sceUserServiceGetInitialUser(int*);
int APS5_VABI sceUserServiceGetForegroundUser(int*);
int APS5_VABI sceUserServiceGetLoginUserIdList(UserServiceLoginUserIdList*);
int APS5_VABI sceUserServiceTerminate();
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    Require(sceUserServiceInitialize(nullptr) == USER_SERVICE_OK);
    struct Guarded { int before; int user; int after; } foreground{123, -1, 456};
    int initial = USER_SERVICE_USER_ID_INVALID;
    UserServiceLoginUserIdList users{};
    Require(sceUserServiceGetInitialUser(&initial) == USER_SERVICE_OK);
    Require(sceUserServiceGetForegroundUser(&foreground.user) == USER_SERVICE_OK);
    Require(sceUserServiceGetLoginUserIdList(&users) == USER_SERVICE_OK);
    Require(foreground.before == 123 && foreground.after == 456);
    Require(foreground.user == initial && initial != USER_SERVICE_USER_ID_INVALID);
    Require(users.user_id[0] == initial);
    for (int index = 1; index < 4; ++index) Require(users.user_id[index] == USER_SERVICE_USER_ID_INVALID);
    Require(sceUserServiceGetForegroundUser(nullptr) == USER_SERVICE_ERROR_INVALID_ARGUMENT);
    Require(sceUserServiceTerminate() == USER_SERVICE_OK);
}
