#include "prx/libc/include/FileStream.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdint>
extern "C" {
extern FileStream* __stdinp_nid_postfix;
extern FileStream* __stdoutp_nid_postfix;
extern FileStream* __stderrp_nid_postfix;
extern int __isthreaded_nid_postfix;
int APS5_VABI fprintf_nid_postfix(FileStream*, const char*, ...);
int APS5_VABI vfprintf_nid_postfix(FileStream*, const char*, void*);
int APS5_VABI fgetc_nid_postfix(FileStream*);
int APS5_VABI fputc_nid_postfix(int, FileStream*);
int APS5_VABI __srget_nid_postfix(FileStream*);
int APS5_VABI __swbuf_nid_postfix(int, FileStream*);
int APS5_VABI ungetc_nid_postfix(int, FileStream*);
char* APS5_VABI fgets_nid_postfix(char*, int, FileStream*);
int APS5_VABI feof_nid_postfix(FileStream*);
int APS5_VABI fileno_nid_postfix(FileStream*);
void APS5_VABI clearerr_nid_postfix(FileStream*);
int APS5_VABI setvbuf_nid_postfix(FileStream*, char*, int, std::size_t);
}
static void Require(bool value) { if (!value) std::abort(); }
static int APS5_VABI WriteFormatted(FileStream* stream, const char* format, ...) {
#ifdef _WIN32
    __builtin_sysv_va_list args;
    __builtin_sysv_va_start(args, format);
#else
    std::va_list args;
    va_start(args, format);
#endif
    const int result = vfprintf_nid_postfix(stream, format, args);
#ifdef _WIN32
    __builtin_sysv_va_end(args);
#else
    va_end(args);
#endif
    return result;
}
int main() {
    Require(__isthreaded_nid_postfix == 1);
    Require(__stdoutp_nid_postfix == &_Stdout_nid_postfix);
    Require(__stderrp_nid_postfix == &_Stderr_nid_postfix);
    Require(fileno_nid_postfix(__stdinp_nid_postfix) == 0);
    Require(fileno_nid_postfix(__stdoutp_nid_postfix) == 1);
    Require(fileno_nid_postfix(__stderrp_nid_postfix) == 2);
    FileStream stream(std::tmpfile());
    auto& guest = *reinterpret_cast<GuestFilePrefix*>(&stream);
    Require(&guest == &stream.GuestState());
    Require(guest.position == nullptr && guest.readRemaining == 0 && guest.writeRemaining == 0);
    Require(guest.descriptor == fileno_nid_postfix(&stream));
    Require(setvbuf_nid_postfix(&stream, nullptr, 2, 0) == 0);
    Require(fputc_nid_postfix('A', &stream) == 'A');
    Require(--guest.writeRemaining < 0 && __swbuf_nid_postfix('\n', &stream) == '\n');
    std::rewind(stream.GetHandle());
    Require(--guest.readRemaining < 0 && __srget_nid_postfix(&stream) == 'A');
    Require(ungetc_nid_postfix('B', &stream) == 'B');
    char text[8]{};
    Require(fgets_nid_postfix(text, sizeof(text), &stream) == text);
    Require(std::strcmp(text, "B\n") == 0);
    Require(fgetc_nid_postfix(&stream) == EOF);
    Require(feof_nid_postfix(&stream) && (guest.flags & 0x20));
    clearerr_nid_postfix(&stream);
    Require(!feof_nid_postfix(&stream) && !(guest.flags & 0x20));
    stream.Close();
    Require(guest.flags == 0 && guest.descriptor == -1);

    FileStream formatted(std::tmpfile());
    const char expected[] = "guest 4294967297 1.25 1 2 3 4 5 6 7 8\n";
    Require(fprintf_nid_postfix(&formatted, "%s %ld %.2f %d %d %d %d %d %d %d %d\n",
        "guest", std::int64_t{4294967297}, 1.25, 1, 2, 3, 4, 5, 6, 7, 8) == sizeof(expected) - 1);
    Require(WriteFormatted(&formatted, "%*.*f:%s", 6, 2, 3.5, "end") == 10);
    std::rewind(formatted.GetHandle());
    char output[128]{};
    Require(fgets_nid_postfix(output, sizeof(output), &formatted) == output);
    Require(std::strcmp(output, expected) == 0);
    Require(fgets_nid_postfix(output, sizeof(output), &formatted) == output);
    Require(std::strcmp(output, "  3.50:end") == 0);
    formatted.Close();
}
