#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdlib>
#include <cstring>
#include <filesystem>
extern "C" {
const char* APS5_VABI getprogname_nid_postfix();
void APS5_VABI setprogname_nid_postfix(const char*);
char* APS5_VABI getenv_nid_postfix(const char*);
int APS5_VABI setenv_nid_postfix(const char*, const char*, int);
int APS5_VABI unsetenv_nid_postfix(const char*);
int APS5_VABI putenv_nid_postfix(char*);
int* APS5_VABI __error_nid_postfix();
}
static void Require(bool value) { if (!value) std::abort(); }
int main(int argc, char** argv) {
    Require(argc > 0 && argv[0]);
    const auto* initial = getprogname_nid_postfix();
    Require(initial && initial == getprogname_nid_postfix());
    Require(std::filesystem::path(argv[0]).filename().string() == initial);
    static const char path[] = "/app0/guest-game";
    setprogname_nid_postfix(path);
    Require(getprogname_nid_postfix() == path + 6);
    static const char plain[] = "renamed";
    setprogname_nid_postfix(plain);
    Require(getprogname_nid_postfix() == plain);
    static const char trailing[] = "/app0/";
    setprogname_nid_postfix(trailing);
    Require(getprogname_nid_postfix() == trailing + 6 && *getprogname_nid_postfix() == '\0');
    static const char empty[] = "";
    setprogname_nid_postfix(empty);
    Require(getprogname_nid_postfix() == empty);
    setprogname_nid_postfix(initial);
    const char* key = "ANYPS5_GUEST_ENV_TEST_4C27";
#ifdef _WIN32
    Require(_putenv_s(key, "inherited") == 0);
#else
    Require(::setenv(key, "inherited", 1) == 0);
#endif
    Require(std::strcmp(getenv_nid_postfix(key), "inherited") == 0);
    Require(setenv_nid_postfix(key, "kept out", 0) == 0);
    Require(std::strcmp(getenv_nid_postfix(key), "inherited") == 0);
    char value[] = "copied";
    Require(setenv_nid_postfix(key, value, 1) == 0);
    value[0] = 'X';
    Require(std::strcmp(getenv_nid_postfix(key), "copied") == 0);
    Require(std::strcmp(std::getenv(key), "inherited") == 0);
    Require(setenv_nid_postfix(key, "", 1) == 0);
    Require(getenv_nid_postfix(key) && *getenv_nid_postfix(key) == 0);
    char borrowed[] = "ANYPS5_GUEST_ENV_TEST_4C27=one";
    Require(putenv_nid_postfix(borrowed) == 0);
    char* position = std::strchr(borrowed, '=') + 1;
    Require(getenv_nid_postfix(key) == position);
    std::memcpy(position, "two", 3);
    Require(std::strcmp(getenv_nid_postfix(key), "two") == 0);
    Require(setenv_nid_postfix("anyps5_guest_env_test_4c27", "lower", 1) == 0);
    Require(std::strcmp(getenv_nid_postfix(key), "two") == 0);
    Require(setenv_nid_postfix("bad=name", "x", 1) == -1);
    Require(*__error_nid_postfix() == 22);
    Require(unsetenv_nid_postfix("") == -1);
    char invalid[] = "no-separator";
    Require(putenv_nid_postfix(invalid) == -1);
    Require(unsetenv_nid_postfix(key) == 0);
    Require(getenv_nid_postfix(key) == nullptr);
    Require(unsetenv_nid_postfix(key) == 0);
    Require(unsetenv_nid_postfix("anyps5_guest_env_test_4c27") == 0);
}
