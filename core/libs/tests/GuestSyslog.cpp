#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <syslog.h>
#define _fileno fileno
#define _dup dup
#define _dup2 dup2
#define _close close
#endif
extern "C" {
void APS5_VABI syslog_nid_postfix(int, const char*, ...);
int APS5_VABI setlogmask_nid_postfix(int);
int* APS5_VABI __error_nid_postfix();
}
static void Require(bool value) { if (!value) std::abort(); }
int main() {
    auto* capture = std::tmpfile();
    Require(capture != nullptr);
    const int saved = _dup(_fileno(stderr));
    Require(saved >= 0 && _dup2(_fileno(capture), _fileno(stderr)) >= 0);
#ifndef _WIN32
    openlog("guest-test", LOG_PERROR, LOG_USER);
#endif
    Require(setlogmask_nid_postfix(0) == 255);
    Require(setlogmask_nid_postfix(1 << 3) == 255);
    *__error_nid_postfix() = 35;
    syslog_nid_postfix(7, "filtered-message");
    syslog_nid_postfix(3, "%s:%ld:%0.2f:%d,%d,%d,%d,%d,%d,%d: %m; %%m",
        "guest", INT64_C(4294967297), 2.5, 1, 2, 3, 4, 5, 6, 7);
    Require(*__error_nid_postfix() == 35);
    Require(setlogmask_nid_postfix(255) == (1 << 3));
    Require(std::fflush(stderr) == 0);
    Require(_dup2(saved, _fileno(stderr)) >= 0);
    Require(_close(saved) == 0);
    std::rewind(capture);
    std::string text;
    char buffer[256];
    while (const auto size = std::fread(buffer, 1, sizeof(buffer), capture)) text.append(buffer, size);
    Require(std::fclose(capture) == 0);
    Require(text.find("filtered-message") == std::string::npos);
    Require(text.find("guest:4294967297:2.50:1,2,3,4,5,6,7: Resource temporarily unavailable; %m") != std::string::npos);
}
