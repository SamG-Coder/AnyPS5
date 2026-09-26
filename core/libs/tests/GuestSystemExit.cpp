#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include "prx/libSceSystemService/SystemService.hpp"
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
extern "C" int APS5_VABI sceSystemServiceLoadExec(const char*, const char* const*);
extern "C" void APS5_VABI _Exit_nid_postfix(int);
extern "C" int APS5_VABI system_nid_postfix(const char*);
using GuestDestructor = void (APS5_VABI *)(void*);
extern "C" int APS5_VABI __cxa_atexit_nid_postfix(GuestDestructor, void*, void*);
namespace {
bool cleaned = false;
int destructorArguments[] = {1, 2};
int destructorCount = 0;
void APS5_VABI GuestCleanup(void* argument) {
    if (destructorCount >= 2 || argument != &destructorArguments[1 - destructorCount])
        std::_Exit(4);
    ++destructorCount;
    if (destructorCount == 2) {
        std::puts("Guest destructors completed in reverse order");
        std::fflush(stdout);
    }
}
void Cleanup() { cleaned = true; }
void VerifyExit() {
    if (!cleaned) std::_Exit(1);
    std::puts("Guest shutdown and atexit completed");
}
void Require(bool value) { if (!value) std::abort(); }
void UnexpectedCleanup() { std::_Exit(3); }
}
int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--destructors") == 0) {
        Require(__cxa_atexit_nid_postfix(GuestCleanup, &destructorArguments[0], nullptr) == 0);
        Require(__cxa_atexit_nid_postfix(GuestCleanup, &destructorArguments[1], nullptr) == 0);
        sceSystemServiceLoadExec("exit", nullptr);
        return 2;
    }
    if (argc > 1 && std::strcmp(argv[1], "--immediate") == 0) {
        LibcRegisterShutdown_nid_postfix(UnexpectedCleanup);
        Require(std::atexit(UnexpectedCleanup) == 0);
        _Exit_nid_postfix(0);
        return 2;
    }
    if (argc > 1) {
        LibcRegisterShutdown_nid_postfix(Cleanup);
        Require(std::atexit(VerifyExit) == 0);
        sceSystemServiceLoadExec("exit", nullptr);
        return 2;
    }
    Require(system_nid_postfix(nullptr) == 0);
    for (const char* command : {"", "exit 0"}) {
        bool unsupported = false;
        try { system_nid_postfix(command); }
        catch (const std::runtime_error& error) {
            unsupported = std::strcmp(error.what(), "system: guest command execution is not supported") == 0;
        }
        Require(unsupported);
    }
    Require(sceSystemServiceLoadExec(nullptr, nullptr) == SYSTEM_SERVICE_ERROR_PARAMETER);
    Require(sceSystemServiceLoadExec("", nullptr) == SYSTEM_SERVICE_ERROR_PARAMETER);
    bool rejected = false;
    try { sceSystemServiceLoadExec("/app0/another.bin", nullptr); }
    catch (const std::runtime_error&) { rejected = true; }
    Require(rejected);
}
