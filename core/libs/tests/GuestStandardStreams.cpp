#include "prx/libc/include/FileStream.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdlib>
#include <cstring>
extern "C" {
extern FileStream* __stdinp_nid_postfix;
extern FileStream* __stdoutp_nid_postfix;
extern FileStream* __stderrp_nid_postfix;
extern int __isthreaded_nid_postfix;
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
}
