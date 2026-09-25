#include "prx/libc/include/general/VabiMacros.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <set>
#include <vector>
#ifndef _WIN32
#include <sys/stat.h>
#endif
extern "C" {
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
