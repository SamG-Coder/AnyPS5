#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libc/include/FileStream.hpp"
#include "SceTypes.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <set>
#include <vector>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <aclapi.h>
#endif
#ifndef _WIN32
#include <sys/stat.h>
#endif
extern "C" {
int APS5_VABI stat_nid_postfix(const char*, FileStat*);
int APS5_VABI sceKernelStat(const char*, FileStat*);
int APS5_VABI mkdir_nid_postfix(const char*, std::uint16_t);
int APS5_VABI sceKernelMkdir(const char*, std::uint16_t);
FileStream* APS5_VABI fdopen_nid_postfix(int, const char*);
int APS5_VABI fclose_nid_postfix(FileStream*);
int APS5_VABI fileno_nid_postfix(FileStream*);
int APS5_VABI access_nid_postfix(const char*, int);
int APS5_VABI chdir_nid_postfix(const char*);
int APS5_VABI mkstemp_nid_postfix(char*);
int APS5_VABI unlink_nid_postfix(const char*);
int APS5_VABI remove_nid_postfix(const char*);
int APS5_VABI rename_nid_postfix(const char*, const char*);
int* APS5_VABI __error_nid_postfix();
}
static void Check(bool value, int line) {
    if (!value) {
        std::fprintf(stderr, "Filesystem check failed at line %d (guest errno %d)\n", line, *__error_nid_postfix());
        std::abort();
    }
}
#define Require(value) Check((value), __LINE__)
int main() {
    const auto root = std::filesystem::path("anyps5-filesystem-test-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    Require(std::filesystem::create_directory(root));
    static_assert(sizeof(FileStat) == 120 && offsetof(FileStat, st_size) == 72);
    struct GuardedStat { FileStat value; std::uint64_t guard = UINT64_MAX; } metadata{};
    *__error_nid_postfix() = 13;
    Require(stat_nid_postfix(root.string().c_str(), &metadata.value) == 0);
    Require((metadata.value.st_mode & 0170000) == 0040000 && metadata.guard == UINT64_MAX);
    Require(*__error_nid_postfix() == 13);
    const auto statFile = root / "metadata.bin";
    { std::ofstream stream(statFile, std::ios::binary); stream << "1234567"; }
    Require(stat_nid_postfix(statFile.string().c_str(), &metadata.value) == 0);
    Require((metadata.value.st_mode & 0170000) == 0100000 && metadata.value.st_size == 7);
    Require(metadata.value.st_nlink >= 1 && metadata.value.st_blksize > 0 && metadata.value.st_mtim.tv_sec > 0);
    const auto savedMetadata = metadata.value;
    Require(stat_nid_postfix((root / "absent").string().c_str(), &metadata.value) == -1 && *__error_nid_postfix() == 2);
    Require(std::memcmp(&savedMetadata, &metadata.value, sizeof(FileStat)) == 0 && metadata.guard == UINT64_MAX);
    Require(stat_nid_postfix(nullptr, &metadata.value) == -1 && *__error_nid_postfix() == 14);
    Require(stat_nid_postfix("", &metadata.value) == -1 && *__error_nid_postfix() == 2);
    Require(stat_nid_postfix(statFile.string().c_str(), nullptr) == -1 && *__error_nid_postfix() == 14);
    Require(sceKernelStat((root / "absent").string().c_str(), &metadata.value) == static_cast<int>(0x80020002u));
    Require(*__error_nid_postfix() == 14);
    Require(sceKernelStat(statFile.string().c_str(), &metadata.value) == 0 && metadata.value.st_size == 7);
    Require(*__error_nid_postfix() == 14);
    Require(remove_nid_postfix(statFile.string().c_str()) == 0);
    const auto directory = (root / "created").string();
    *__error_nid_postfix() = 13;
    Require(mkdir_nid_postfix(directory.c_str(), 0700) == 0);
    Require(std::filesystem::is_directory(directory) && *__error_nid_postfix() == 13);
    Require(mkdir_nid_postfix(directory.c_str(), 0700) == -1 && *__error_nid_postfix() == 17);
    Require(sceKernelMkdir(directory.c_str(), 0700) == static_cast<int>(0x80020011u));
    Require(*__error_nid_postfix() == 17);
    Require(mkdir_nid_postfix((root / "missing" / "child").string().c_str(), 0700) == -1 && *__error_nid_postfix() == 2);
    Require(!std::filesystem::exists(root / "missing"));
    Require(mkdir_nid_postfix(nullptr, 0700) == -1 && *__error_nid_postfix() == 14);
    Require(mkdir_nid_postfix("", 0700) == -1 && *__error_nid_postfix() == 2);
    const auto nested = (root / "created" / "nested").string();
    Require(sceKernelMkdir(nested.c_str(), 0700) == 0 && std::filesystem::is_directory(nested));
    Require(remove_nid_postfix(nested.c_str()) == 0);
    Require(remove_nid_postfix(directory.c_str()) == 0);
    Require(fdopen_nid_postfix(-1, "r") == nullptr && *__error_nid_postfix() == 9);
    Require(fdopen_nid_postfix(0, "invalid") == nullptr && *__error_nid_postfix() == 22);
    auto fdPattern = (root / "fdopen-XXXXXX").string();
    const int fd = mkstemp_nid_postfix(fdPattern.data());
    Require(fd >= 0);
    Require(fdopen_nid_postfix(fd, "bad") == nullptr && *__error_nid_postfix() == 22);
    auto* guestStream = fdopen_nid_postfix(fd, "w+");
    Require(guestStream && fileno_nid_postfix(guestStream) == fd);
    const unsigned char binary[] = {10, 26, 0, 255};
    Require(std::fwrite(binary, 1, sizeof(binary), guestStream->GetHandle()) == sizeof(binary));
    Require(std::fflush(guestStream->GetHandle()) == 0);
    Require(std::filesystem::file_size(fdPattern) == sizeof(binary));
    Require(fclose_nid_postfix(guestStream) == 0);
    Require(fdopen_nid_postfix(fd, "r") == nullptr && *__error_nid_postfix() == 9);
#ifdef _WIN32
    const int appendFd = _open(fdPattern.c_str(), _O_RDWR | _O_BINARY);
#else
    const int appendFd = ::open(fdPattern.c_str(), O_RDWR);
#endif
    Require(appendFd >= 0);
    guestStream = fdopen_nid_postfix(appendFd, "a");
    Require(guestStream != nullptr);
    Require(std::fseek(guestStream->GetHandle(), 0, SEEK_SET) == 0);
    Require(std::fwrite(binary, 1, sizeof(binary), guestStream->GetHandle()) == sizeof(binary));
    Require(fclose_nid_postfix(guestStream) == 0);
    Require(std::filesystem::file_size(fdPattern) == 2 * sizeof(binary));
    Require(unlink_nid_postfix(fdPattern.c_str()) == 0);
    Require(mkstemp_nid_postfix(nullptr) == -1 && *__error_nid_postfix() == 14);
    char invalid[] = "temp-XXXXX";
    Require(mkstemp_nid_postfix(invalid) == -1 && *__error_nid_postfix() == 22);
    Require(std::strcmp(invalid, "temp-XXXXX") == 0);
    auto missing = (root / "missing" / "temp-XXXXXX").string();
    Require(mkstemp_nid_postfix(missing.data()) == -1 && *__error_nid_postfix() == 2);
    std::set<std::string> temporaryPaths;
    std::vector<FILE*> streams;
    for (unsigned index = 0; index < 32; ++index) {
        auto pattern = (root / "temp-XXXXXX").string();
        const auto prefix = pattern.substr(0, pattern.size() - 6);
        const int descriptor = mkstemp_nid_postfix(pattern.data());
        Require(descriptor >= 0 && pattern.substr(0, pattern.size() - 6) == prefix);
        Require(temporaryPaths.insert(pattern).second);
#ifdef _WIN32
        auto* stream = ::_fdopen(descriptor, "w+b");
#else
        struct stat metadata{};
        Require(::fstat(descriptor, &metadata) == 0 && (metadata.st_mode & 077) == 0);
        auto* stream = ::fdopen(descriptor, "w+b");
#endif
        Require(stream != nullptr);
        streams.push_back(stream);
        const unsigned char expected[] = {0, 10, 26, 255};
        Require(std::fwrite(expected, 1, sizeof(expected), stream) == sizeof(expected));
        Require(std::fflush(stream) == 0 && std::filesystem::file_size(pattern) == sizeof(expected));
        std::rewind(stream);
        unsigned char result[sizeof(expected)]{};
        Require(std::fread(result, 1, sizeof(result), stream) == sizeof(result));
        Require(std::memcmp(result, expected, sizeof(result)) == 0);
    }
    for (auto* stream : streams) Require(std::fclose(stream) == 0);
    for (const auto& path : temporaryPaths) Require(unlink_nid_postfix(path.c_str()) == 0);
    Require(unlink_nid_postfix(nullptr) == -1 && *__error_nid_postfix() == 14);
    Require(unlink_nid_postfix("") == -1 && *__error_nid_postfix() == 2);
    Require(unlink_nid_postfix(root.string().c_str()) == -1);
    Require(std::filesystem::is_directory(root));
    const auto original = root / "original";
    const auto alias = root / "alias";
    { std::ofstream stream(original); stream << "preserved hard link"; }
    std::filesystem::create_hard_link(original, alias);
    Require(unlink_nid_postfix(original.string().c_str()) == 0);
    Require(!std::filesystem::exists(original));
    { std::ifstream stream(alias); std::string contents; std::getline(stream, contents);
      Require(contents == "preserved hard link"); }
    Require(unlink_nid_postfix(alias.string().c_str()) == 0);
    Require(unlink_nid_postfix(alias.string().c_str()) == -1 && *__error_nid_postfix() == 2);
    const auto file = root / "file.txt";
    { std::ofstream stream(file); stream << "retained until removal"; }
    Require(access_nid_postfix(nullptr, 0) == -1 && *__error_nid_postfix() == 14);
    Require(access_nid_postfix("", 0) == -1 && *__error_nid_postfix() == 2);
    Require(access_nid_postfix(file.string().c_str(), 8) == -1 && *__error_nid_postfix() == 22);
    Require(access_nid_postfix((root / "missing").string().c_str(), 0) == -1 && *__error_nid_postfix() == 2);
    Require(access_nid_postfix(file.string().c_str(), 0) == 0);
    Require(access_nid_postfix(file.string().c_str(), 6) == 0);
    Require(access_nid_postfix(root.string().c_str(), 1) == 0);
    Require(chdir_nid_postfix(root.string().c_str()) == 0);
    Require(access_nid_postfix("file.txt", 4) == 0);
    Require(access_nid_postfix(("/" + file.generic_string()).c_str(), 4) == 0);
    Require(chdir_nid_postfix("/") == 0);
#ifdef _WIN32
    const auto nativeFile = std::filesystem::absolute(file).wstring();
    Require(SetFileAttributesW(nativeFile.c_str(), FILE_ATTRIBUTE_READONLY));
    Require(access_nid_postfix(file.string().c_str(), 4) == 0);
    Require(access_nid_postfix(file.string().c_str(), 2) == -1 && *__error_nid_postfix() == 13);
    Require(SetFileAttributesW(nativeFile.c_str(), FILE_ATTRIBUTE_NORMAL));
    PSECURITY_DESCRIPTOR security = nullptr;
    PACL originalAcl = nullptr;
    Require(GetNamedSecurityInfoW(nativeFile.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
        nullptr, nullptr, &originalAcl, nullptr, &security) == ERROR_SUCCESS);
    ACL denied{};
    Require(InitializeAcl(&denied, sizeof(denied), ACL_REVISION));
    Require(SetNamedSecurityInfoW(const_cast<wchar_t*>(nativeFile.c_str()), SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
        nullptr, nullptr, &denied, nullptr) == ERROR_SUCCESS);
    Require(access_nid_postfix(file.string().c_str(), 0) == 0);
    for (int mode : {1, 2, 4, 7})
        Require(access_nid_postfix(file.string().c_str(), mode) == -1 && *__error_nid_postfix() == 13);
    Require(SetNamedSecurityInfoW(const_cast<wchar_t*>(nativeFile.c_str()), SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION | UNPROTECTED_DACL_SECURITY_INFORMATION,
        nullptr, nullptr, originalAcl, nullptr) == ERROR_SUCCESS);
    LocalFree(security);
    Require(access_nid_postfix(file.string().c_str(), 6) == 0);
#endif
    Require(remove_nid_postfix(root.string().c_str()) == -1);
    Require(*__error_nid_postfix() == 66);
    Require(std::filesystem::is_regular_file(file));
    Require(remove_nid_postfix((file / "invalid").string().c_str()) == -1);
    Require(remove_nid_postfix("") == -1 && *__error_nid_postfix() == 2);
    Require(remove_nid_postfix(nullptr) == -1 && *__error_nid_postfix() == 14);
    const auto renamed = root / "renamed.txt";
    { std::ofstream stream(renamed); stream << "old contents"; }
    Require(rename_nid_postfix(file.string().c_str(), renamed.string().c_str()) == 0);
    Require(!std::filesystem::exists(file));
    { std::ifstream stream(renamed); std::string contents; std::getline(stream, contents);
      Require(contents == "retained until removal"); }
    Require(rename_nid_postfix(renamed.string().c_str(), renamed.string().c_str()) == 0);
    Require(rename_nid_postfix(file.string().c_str(), renamed.string().c_str()) == -1);
    Require(*__error_nid_postfix() == 2);
    Require(rename_nid_postfix(renamed.string().c_str(), file.string().c_str()) == 0);
    Require(remove_nid_postfix(file.string().c_str()) == 0);
    Require(!std::filesystem::exists(file));
    Require(remove_nid_postfix(file.string().c_str()) == -1 && *__error_nid_postfix() == 2);
    Require(remove_nid_postfix(root.string().c_str()) == 0);
    Require(!std::filesystem::exists(root));
}
