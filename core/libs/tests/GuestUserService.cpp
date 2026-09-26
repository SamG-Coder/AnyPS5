#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libSceUserService/UserService.hpp"
#include "SceTypes.hpp"
#include <cstdlib>
#include <array>
#include <cstring>

extern "C" {
int APS5_VABI sceUserServiceInitialize(const void*);
int APS5_VABI sceUserServiceGetInitialUser(int*);
int APS5_VABI sceUserServiceGetForegroundUser(int*);
int APS5_VABI sceUserServiceGetLoginUserIdList(UserServiceLoginUserIdList*);
int APS5_VABI sceUserServiceTerminate();
int APS5_VABI sceUserServiceGetUserName(int, char*, std::size_t);
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
    static_assert(USER_SERVICE_ERROR_INVALID_ARGUMENT == static_cast<int>(0x80960005u));
    std::array<char, 32> name;
    name.fill('!');
    const auto unchanged = name;
    Require(sceUserServiceGetUserName(initial, nullptr, name.size()) == USER_SERVICE_ERROR_INVALID_ARGUMENT);
    Require(sceUserServiceGetUserName(-1, name.data(), name.size()) == USER_SERVICE_ERROR_INVALID_ARGUMENT);
    Require(sceUserServiceGetUserName(initial + 1, name.data(), name.size()) == USER_SERVICE_ERROR_NOT_LOGGED_IN);
    for (std::size_t size = 0; size < sizeof(USER_SERVICE_INITIAL_USER_NAME); ++size)
        Require(sceUserServiceGetUserName(initial, name.data(), size) == USER_SERVICE_ERROR_BUFFER_TOO_SHORT);
    Require(name == unchanged);
    Require(sceUserServiceGetUserName(initial, name.data(), sizeof(USER_SERVICE_INITIAL_USER_NAME)) == 0);
    Require(std::strcmp(name.data(), "Player") == 0);
    for (std::size_t index = sizeof(USER_SERVICE_INITIAL_USER_NAME); index < name.size(); ++index)
        Require(name[index] == '!');
    name.fill('!');
    Require(sceUserServiceGetUserName(foreground.user, name.data(), name.size()) == 0);
    Require(std::strcmp(name.data(), "Player") == 0);
    Require(sceUserServiceTerminate() == USER_SERVICE_OK);
}
