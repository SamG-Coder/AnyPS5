#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>
#include <cstdlib>
#include <array>
#include <cstring>
#include <vector>
#include "prx/libc/include/General.hpp"
extern "C" {
std::int64_t APS5_VABI sysconf_nid_postfix(int);
int APS5_VABI getpagesize_nid_postfix();
int* APS5_VABI __error_nid_postfix();
int APS5_VABI sysctl_nid_postfix(const int*, unsigned, void*, std::size_t*, const void*, std::size_t);
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    *__error_nid_postfix() = 13;
    Require(sysconf_nid_postfix(47) == 0x4000);
    Require(getpagesize_nid_postfix() == sysconf_nid_postfix(47));
    Require(sysconf_nid_postfix(57) > 0);
    Require(sysconf_nid_postfix(58) > 0);
    Require(sysconf_nid_postfix(121) > 0);
    Require(*__error_nid_postfix() == 13);
    Require(sysconf_nid_postfix(-1) == -1); // verifies full-width signed return
    Require(*__error_nid_postfix() == 22);
    Require(sysconf_nid_postfix(0x7fffffff) == -1);
    for (const int identifier : {3, 7}) {
        const int name[] = {6, identifier};
        std::size_t size = 0;
        *__error_nid_postfix() = 13;
        Require(sysctl_nid_postfix(name, 2, nullptr, &size, nullptr, 0) == 0 && size == 4);
        std::int32_t value = 0;
        Require(sysctl_nid_postfix(name, 2, &value, &size, nullptr, 0) == 0);
        Require(value == sysconf_nid_postfix(identifier == 3 ? 58 : 47));
        Require(*__error_nid_postfix() == 13);
        std::array<unsigned char, 4> small{0xff, 0xff, 0xff, 0xff};
        size = 2;
        Require(sysctl_nid_postfix(name, 2, small.data(), &size, nullptr, 0) == -1);
        Require(*__error_nid_postfix() == 12 && size == 2);
        Require(std::memcmp(small.data(), &value, 2) == 0 && small[2] == 0xff && small[3] == 0xff);
        Require(sysctl_nid_postfix(name, 2, &value, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
        Require(sysctl_nid_postfix(name, 2, nullptr, nullptr, &value, sizeof(value)) == -1 && *__error_nid_postfix() == 1);
    }
    const int unknown[] = {6, 0x7fffffff};
    std::size_t unchanged = 77;
    Require(sysctl_nid_postfix(unknown, 2, nullptr, &unchanged, nullptr, 0) == -1);
    Require(*__error_nid_postfix() == 2 && unchanged == 77);
    Require(sysctl_nid_postfix(nullptr, 2, nullptr, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 14);
    Require(sysctl_nid_postfix(unknown, 25, nullptr, nullptr, nullptr, 0) == -1 && *__error_nid_postfix() == 22);
    const int executable[] = {1, 14, 12, -1};
    std::size_t size = 0;
    Require(sysctl_nid_postfix(executable, 4, nullptr, &size, nullptr, 0) == 0 && size > 1);
    std::vector<char> path(size);
    Require(sysctl_nid_postfix(executable, 4, path.data(), &size, nullptr, 0) == 0);
    Require(path.front() == '/' && path.back() == '\0' && std::strlen(path.data()) + 1 == size);
    Require(std::filesystem::is_regular_file(ResolvePath_nid_no_patch(path.data())));
}
