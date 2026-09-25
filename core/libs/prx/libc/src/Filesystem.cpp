#include "prx/libc/include/General.hpp"
#include <cerrno>
#include <cstring>
#include <random>
#include <fcntl.h>
#include <sys/stat.h>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace {
int FilesystemError(const std::error_code& error) {
    if (error == std::errc::no_such_file_or_directory) return 2;
    if (error == std::errc::permission_denied) return 13;
    if (error == std::errc::operation_not_permitted) return 1;
    if (error == std::errc::not_a_directory) return 20;
    if (error == std::errc::is_a_directory) return 21;
    if (error == std::errc::directory_not_empty) return 66;
    if (error == std::errc::device_or_resource_busy) return 16;
    if (error == std::errc::read_only_file_system) return 30;
    if (error == std::errc::filename_too_long) return 63;
    if (error == std::errc::too_many_symbolic_link_levels) return 62;
    if (error == std::errc::not_enough_memory) return 12;
    if (error == std::errc::invalid_argument) return 22;
    if (error == std::errc::cross_device_link) return 18;
    if (error == std::errc::file_exists) return 17;
    if (error == std::errc::no_space_on_device) return 28;
    if (error == std::errc::too_many_files_open) return 24;
    if (error == std::errc::too_many_files_open_in_system) return 23;
    return 5;
}
}

extern "C" int APS5_VABI mkstemp_nid_postfix(char* pattern) {
    if (!pattern) { errno = 14; return -1; }
    const auto length = std::strlen(pattern);
    if (length < 6 || std::strcmp(pattern + length - 6, "XXXXXX") != 0) {
        errno = 22;
        return -1;
    }
    try {
        std::string candidate(pattern);
        std::random_device random;
        std::uniform_int_distribution<unsigned> character(0, 61);
        constexpr char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (unsigned attempt = 0; attempt < 256; ++attempt) {
            for (auto index = length - 6; index < length; ++index)
                candidate[index] = alphabet[character(random)];
            const auto resolved = ResolvePath_nid_no_patch(candidate.c_str());
#ifdef _WIN32
            const int descriptor = ::_wopen(resolved.c_str(), _O_RDWR | _O_CREAT | _O_EXCL | _O_BINARY,
                _S_IREAD | _S_IWRITE);
#else
            const int descriptor = ::open(resolved.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
#endif
            if (descriptor >= 0) {
                std::memcpy(pattern + length - 6, candidate.data() + length - 6, 6);
                return descriptor;
            }
            if (errno != EEXIST) {
                errno = FilesystemError(std::error_code(errno, std::generic_category()));
                return -1;
            }
        }
        errno = 17;
        return -1;
    } catch (const std::bad_alloc&) { errno = 12; return -1; }
      catch (const std::filesystem::filesystem_error& error) {
        errno = FilesystemError(error.code());
        return -1;
    }
}

extern "C" int APS5_VABI rename_nid_postfix(const char* from, const char* to) {
    if (!from || !to) { errno = 14; return -1; }
    if (!*from || !*to) { errno = 2; return -1; }
    try {
        const auto source = ResolvePath_nid_no_patch(from);
        const auto destination = ResolvePath_nid_no_patch(to);
        std::error_code error;
        std::filesystem::rename(source, destination, error);
        if (error) { errno = FilesystemError(error); return -1; }
        return 0;
    } catch (const std::bad_alloc&) { errno = 12; return -1; }
      catch (const std::filesystem::filesystem_error& error) {
        errno = FilesystemError(error.code());
        return -1;
    }
}

extern "C" int APS5_VABI unlink_nid_postfix(const char* path) {
    if (!path) { errno = 14; return -1; }
    if (!*path) { errno = 2; return -1; }
    try {
        const auto resolved = ResolvePath_nid_no_patch(path);
#ifdef _WIN32
        const auto handle = CreateFileW(resolved.c_str(), DELETE | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
            FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            errno = FilesystemError(std::error_code(GetLastError(), std::system_category()));
            return -1;
        }
        BY_HANDLE_FILE_INFORMATION information{};
        if (!GetFileInformationByHandle(handle, &information)) {
            const auto error = GetLastError();
            CloseHandle(handle);
            errno = FilesystemError(std::error_code(error, std::system_category()));
            return -1;
        }
        if ((information.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(information.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            CloseHandle(handle);
            errno = 1;
            return -1;
        }
        FILE_DISPOSITION_INFO disposition{TRUE};
        const bool removed = SetFileInformationByHandle(handle, FileDispositionInfo,
            &disposition, sizeof(disposition)) != 0;
        const auto error = removed ? ERROR_SUCCESS : GetLastError();
        CloseHandle(handle);
        if (!removed) {
            errno = FilesystemError(std::error_code(error, std::system_category()));
            return -1;
        }
#else
        if (::unlink(resolved.c_str()) != 0) {
            errno = FilesystemError(std::error_code(errno, std::generic_category()));
            return -1;
        }
#endif
        return 0;
    } catch (const std::bad_alloc&) { errno = 12; return -1; }
      catch (const std::filesystem::filesystem_error& error) {
        errno = FilesystemError(error.code());
        return -1;
    }
}

extern "C" int APS5_VABI remove_nid_postfix(const char* path) {
    if (!path) { errno = 14; return -1; }
    if (!*path) { errno = 2; return -1; }
    try {
        const auto resolved = ResolvePath_nid_no_patch(path);
        std::error_code error;
#ifdef _WIN32
        const DWORD attributes = GetFileAttributesW(resolved.c_str());
        bool removed = false;
        if (attributes != INVALID_FILE_ATTRIBUTES) {
            removed = (attributes & FILE_ATTRIBUTE_DIRECTORY) ?
                RemoveDirectoryW(resolved.c_str()) != 0 : DeleteFileW(resolved.c_str()) != 0;
        }
        if (!removed) {
            const DWORD nativeError = GetLastError();
            if (nativeError == ERROR_DIR_NOT_EMPTY) { errno = 66; return -1; }
            error = std::error_code(static_cast<int>(nativeError), std::system_category());
        }
#else
        const bool removed = std::filesystem::remove(resolved, error);
#endif
        if (error || !removed) {
            errno = error ? FilesystemError(error) : 2;
            return -1;
        }
        return 0;
    } catch (const std::bad_alloc&) { errno = 12; return -1; }
      catch (const std::filesystem::filesystem_error& error) {
        errno = FilesystemError(error.code());
        return -1;
    }
}
