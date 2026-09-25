#include <cstdint>
#include <cstddef>
#include <cerrno>
#include <cstdlib>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libkernel/Socket/include/SocketRuntime.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#ifdef __MINGW32__
extern "C" __declspec(dllimport) _invalid_parameter_handler __cdecl
_set_thread_local_invalid_parameter_handler(_invalid_parameter_handler);
#endif
#else
#include <unistd.h>
#endif

extern "C" {

int APS5_VABI isatty_nid_postfix(int descriptor) {
    if (descriptor >= GuestSockets::FirstDescriptor) {
        errno = GuestSockets::IsOpen(descriptor) ? 25 : 9;
        return 0;
    }
#ifdef _WIN32
    const auto previous = _set_thread_local_invalid_parameter_handler(
        [](const wchar_t*, const wchar_t*, const wchar_t*, unsigned, uintptr_t) {});
    const auto native = _get_osfhandle(descriptor);
    _set_thread_local_invalid_parameter_handler(previous);
    if (native == -1 || native == -2) { errno = 9; return 0; }
    DWORD mode = 0;
    if (GetConsoleMode(reinterpret_cast<HANDLE>(native), &mode)) return 1;
    errno = 25;
    return 0;
#else
    const int result = ::isatty(descriptor);
    if (!result) errno = errno == EBADF ? 9 : 25;
    return result;
#endif
}

int APS5_VABI chmod_nid_postfix(const char* path, int mode) {
 (void)path;
 (void)mode;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI close_nid_postfix(int d) {
    if (d >= GuestSockets::FirstDescriptor) return GuestSockets::Close(d);
#ifdef _WIN32
    return _close(d);
#else
    return ::close(d);
#endif
}

int APS5_VABI flock_nid_postfix(int d, int operation) {
 (void)d;
 (void)operation;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI fstat_nid_disambig1_nid_postfix(int d, FileStat* sb) {
 (void)d;
 (void)sb;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI ftruncate_nid_postfix(int d, int64_t length) {
 (void)d;
 (void)length;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI lseek_nid_postfix(int d, int64_t offset, int whence) {
 (void)d;
 (void)offset;
 (void)whence;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI mkdir_nid_postfix(const char* path, uint16_t mode) {
 (void)path;
 (void)mode;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI open_nid_postfix(const char* path, int flags, int mode) {
 (void)path;
 (void)flags;
 (void)mode;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI pread_nid_postfix(int d, void* buf, size_t nbytes, int64_t offset) {
 (void)d;
 (void)buf;
 (void)nbytes;
 (void)offset;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI pwrite_nid_disambig1_nid_postfix(int d, const void* buf, size_t nbytes, int64_t offset) {
 (void)d;
 (void)buf;
 (void)nbytes;
 (void)offset;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI read_nid_postfix(int d, void* buf, uint64_t nbytes) {
 (void)d;
 (void)buf;
 (void)nbytes;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI write_nid_postfix(int d, const char* str, int64_t size) {
 (void)d;
 (void)str;
 (void)size;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI stat_nid_postfix(const char* path, FileStat* sb) {
 (void)path;
 (void)sb;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelCheckReachability(const char* path) {
 (void)path;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelFstat(int d, FileStat* sb) {
 (void)d;
 (void)sb;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelFsync(int fd) {
 (void)fd;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelGetdents(int fd, char* buf, int nbytes) {
 (void)fd;
 (void)buf;
 (void)nbytes;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelGetdirentries(int fd, char* buf, int nbytes, int64_t* basep) {
 (void)fd;
 (void)buf;
 (void)nbytes;
 (void)basep;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelMkdir(const char* path, uint16_t mode) {
 (void)path;
 (void)mode;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI sceKernelPread(int d, void* buf, size_t nbytes, int64_t offset) {
 (void)d;
 (void)buf;
 (void)nbytes;
 (void)offset;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int64_t APS5_VABI sceKernelPwrite(int d, const void* buf, size_t nbytes, int64_t offset) {
 (void)d;
 (void)buf;
 (void)nbytes;
 (void)offset;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelRename(const char* from, const char* to) {
 (void)from;
 (void)to;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceKernelRmdir(const char* path) {
 (void)path;
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

}
