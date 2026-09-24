# Yamagi Quake II compatibility investigation

Target: [BlackBearReloaded's native PS5 release v0.2.0-alpha.1](https://github.com/blackbearreloaded/ps5-yamagi-quake2/releases/tag/v0.2.0-alpha.1), title PPSA99007. Investigated on Windows on 2026-09-25.

The release includes the Quake II demo and its original notices. All downloaded
game files, extracted executables, and logs remain under ignored `downloads/`.

## Input preparation

`PPSA99007.zip` SHA-256:
`4FA5DA07498FE64FFC67229EED2347E0DDFFD5073F1FEA248BA5E69307229A70`.

The released `eboot.bin` is a plaintext FSELF container. AnyPS5's raw ELF reader
rejects it with `Invalid ELF magic number`. Extract the ELF using the game's
[native application tooling](https://github.com/blackbearreloaded/ps5-native-app-boilerplate),
revision `722f2227a8bb6fa2229120546995b6562552c752`:

```powershell
downloads/ps5-yamagi-quake2/ps5-native-tool.exe self --extract `
  --file downloads/ps5-yamagi-quake2/extracted/PPSA99007/eboot.bin `
  --out downloads/ps5-yamagi-quake2/eboot.elf
```

The extraction tool was built with MSYS2 UCRT64 g++, C++20, and zlib from
`native_app_builder.cpp`, `self_container.cpp`, `elf_object.cpp`, and
`sce_module_writer.cpp` in that repository's `tooling/native/` directory.

Extracted ELF SHA-256:
`FC0879B50EF23B604FA81C8C28843C75AD54A515A6944D8809327C3A51B40A27`.

## Conversion and startup

The ELF contains an empty PT_TLS segment (zero file and memory sizes, alignment
one). The Windows TLS builder previously rejected this. It now accepts an empty
segment when no guest TLS accesses exist, without adding a PE TLS directory.
Malformed size combinations and accesses to an empty segment remain errors.

```powershell
build-winlibs/core/relinker/relinker.exe --windows --windows-diagnostics `
  --rpath D:/AnyPS5/build-winlibs/core/libs/libs `
  downloads/ps5-yamagi-quake2/eboot.elf `
  downloads/ps5-yamagi-quake2/quake2.exe
```

Conversion succeeds with all 382 import references preserved. This command
embeds a workstation-specific library path. Run it from the download directory
if the generated `windows-diagnostics-imports.txt` should also remain there
(adjust the relative paths accordingly).

The compatibility libraries were built using the project's documented WinLibs
GCC 15.2.0 toolchain. Place its `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and
`libwinpthread-1.dll` beside the generated executable for this startup check.

Initial startup result (before the follow-up changes below):

- Loads libSceSystemService, libSceUserService, libScePad, libSceAudioOut,
  libSceVideoOut, libSceKeyboard, libSceImeDialog, libSceAgc, libSceAgcDriver,
  and libSceLibcInternal compatibility modules successfully.
- Stops loading `libScePosixForWebKit.prx`, which AnyPS5 does not currently build.
- Exits with `0xc0000135`; no transfer to the guest entry point and no game frame.

Successful module loading alone does not establish API behavior, rendering,
or playability.

## Follow-up: string exports and Windows runtime visibility

The static import audit found 373 unique imported NIDs (382 relocation
references). Before this follow-up, 168 NIDs were absent from the combined
export tables of the built compatibility modules. This is an export-availability
check, not proof that the other implementations satisfy the game's requirements.

The seven imports attributed to libScePosixForWebKit are `getaddrinfo`,
`freeaddrinfo`, `gai_strerror`, `getnameinfo`, `isatty`, `mkstemp`, and `strcasestr`.
The new module implements `strcasestr`; the other six remain unsupported.
The shared libc also gains `strnlen`, `strncat`, `strpbrk`, `strcspn`, `strlcat`,
and `strtok_r`. None of these additions is a success-returning placeholder.

After adding the module, startup exposed a separate Windows lookup problem:
`_init_env` exists in AnyPS5's libc.prx, but GetProcAddress does not search the
dependencies of libSceLibcInternal.prx. For Windows conversion, a request for
libSceLibcInternal.prx now also adds libc.prx to the explicit symbol search list,
after the game's requested modules. An existing explicit libc.prx entry is
retained without duplication; unrelated inputs gain no dependency.

Latest verified startup loads all twelve game-requested modules plus libc.prx.
It then stops at missing import `BPE9s9vQQXo` (`mmap`) with exit `0xc0000139`.
There is still no transfer to the guest entry point and no game frame. The next
runtime work is guest memory-mapping support, alongside the remaining import
gaps in file I/O, threading, libc, POSIX services, and graphics.

## Regression coverage

`ctest --test-dir build-winlibs --output-on-failure`: four suites pass. The new
`empty_tls` integration suite checks absent PE TLS for an empty guest segment,
retained TLS for a nonempty segment, rejection of malformed sizes and empty TLS
accesses, and execution of both valid generated Windows fixtures (exit 42).

After the follow-up: five suites pass. `guest_strings` exercises the guest SysV
calling convention, bounded unterminated input, concatenation truncation,
interleaved tokenizer state, and case-insensitive searches. Windows dependency
tests also cover implicit libc visibility, ordering, duplicate avoidance, and
unrelated inputs. The complete `libs` target rebuild succeeds.

## Follow-up: anonymous memory mapping

The game's `tooling/native/app_heap.c` requests private anonymous read/write
memory for its OpenGL heap. `mmap` and `munmap` now route this use through
AnyPS5's existing tracked guest allocations, with 16 KiB page rounding and
partial unmapping. Guest MAP_ANON is 0x1000, not the Linux host value 0x20.
Failures return MAP_FAILED or -1 and set guest errno through `__error`.
Shared, fixed-address, and file-backed mappings remain explicitly unsupported
(guest EOPNOTSUPP 45). Unmapping currently requires a contiguous tracked range
within one allocation; holes, foreign memory, and pinned ranges are rejected.

Six CTest suites pass, including real allocations, zero-fill, alignment,
read/write access, tracking, partial unmapping, protection metadata, and invalid
argument/unsupported-mode errors. The unchanged converted game now resolves
the memory-mapping imports and stops at `-hn1tcVHq5Q` (`sceLibcMspaceCreate`).
It still has not reached its entry point or drawn a frame.

## Follow-up: caller-backed heaps and address resolution

The generic `sceLibcMspace*` implementation allocates within the memory region
provided by the caller. It supports malloc, calloc, realloc, free, aligned
allocation, usable-size queries, and destruction. Free blocks coalesce; failed
reallocation preserves the original block. Tests cover region ownership,
alignment, overflow, exhaustion, data preservation, block reuse, and concurrent
calls. Only creation flags zero are currently supported. Arena lifetime remains
the caller's responsibility: destroy it before releasing its backing memory.
No game code or game-specific heap size is embedded in this implementation.

The POSIX module now supplies `getnameinfo`, `getaddrinfo`, `freeaddrinfo`, and
`gai_strerror`, converting guest sockaddr/addrinfo layouts, IPv6 family IDs,
flags, and resolver error values. Native results are copied into owned guest
layout structures and released through the matching free function. Short
getnameinfo buffers return guest EAI_OVERFLOW without copying partial results.
Supported families are IPv4/IPv6; socket hints support unspecified, stream,
and datagram. Unsupported flags and invalid hints fail explicitly.

All eight CTest suites pass on Windows. Resolver tests use numeric loopback
addresses and ports, so they do not depend on external DNS. Live DNS and online
multiplayer are not validated by these tests. The full compatibility-library
build succeeds, and the unchanged game now stops at `RIa6GnWp+iU` (`strerror`).
No guest entry-point execution or rendered frame has been observed yet.

## Follow-up: error reporting and IP conversion

Added generic `strerror`, POSIX `strerror_r`, and `perror` using guest errno
numbering. Tests cover every defined guest errno value, thread-local message
storage, unknown errors, short buffers, and errno preservation. Messages are
currently English; locale-dependent message translation is not implemented.

Added `__inet_pton` and `__inet_ntop` with guest IPv4/IPv6 family translation,
network-byte-order buffers, invalid-input handling, and guest error codes.
Tests exercise IPv4, IPv6, IPv4-mapped IPv6, untouched outputs on failure, and
short buffers. All ten CTest suites pass and the full library build succeeds.

Latest startup stops at `PfccT7qURYE` (`ioctl`). The existing libkernel socket
operations include unimplemented placeholders, so the socket lifecycle needs
review before networking behavior can be considered supported. No gameplay
or guest entry-point execution is established by resolving these imports.

## Follow-up: tracked UDP sockets

Replaced the UDP-path placeholders with tracked guest descriptors, IPv4/IPv6
address conversion, bind, getsockname, sendto, recvfrom, and close. Added guest
FIONBIO/FIONREAD ioctl translation and integer socket options for reuse-address,
broadcast, send-buffer size, and receive-buffer size. Descriptors occupy a
separate range from host CRT file descriptors and are not reused. In-flight
operations retain native socket ownership while guest close removes the handle.
Allocation failure closes the native socket and reports guest ENOMEM.

The new loopback test sends and receives a real IPv4 UDP datagram through guest
exports, checks nonblocking EAGAIN, queued-byte counts, MSG_PEEK, broadcast
options, and closed-descriptor rejection. All eleven suites pass. IPv6 socket
address translation is implemented but this UDP test exercises IPv4 only.
TCP, select, connected send/recv, and unimplemented options remain unsupported;
online multiplayer is not validated. ioctl currently accepts tracked sockets
only. POSIX close also closes ordinary host CRT descriptors.

The unchanged game resolves ioctl and now stops at `mkawd0NA9ts` (`sysconf`),
before guest entry-point execution. No rendered frame has been observed.

## Follow-up: configuration queries and signal callbacks

Added sysconf for guest page size, configured/online processor counts, and
physical page count. The return type is explicitly 64-bit for the guest ABI.
Memory capacity is reported in 16 KiB guest pages; CPU and capacity values
reflect the executing host. Unsupported queries return -1 with guest EINVAL.
The previous getpagesize placeholder now returns the same guest page size.

Added signal/raise bridges for SIGINT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, and
SIGTERM. Host callbacks dispatch through the guest SysV calling convention;
default/ignore handlers and guest-to-host signal numbers are translated.
Tests cover repeated explicit SIGTERM delivery, ignore/default restoration,
and invalid signals. This does not validate hardware-fault recovery, signal
masks, sigaction, or complete asynchronous POSIX semantics.

All thirteen suites pass. Latest game startup resolves these imports and stops
at `zqJhBxAKfsc` (`__stdoutp`). The next work requires the guest FILE layout and
standard stream exports; mapping a host FILE pointer directly would be invalid.
The game still has not executed its entry point or rendered a frame.

## Follow-up: standard stream globals and character I/O

Added __stdinp, __stdoutp, __stderrp, and __isthreaded exports. FileStream now
starts with the verified macro-accessed guest FILE prefix (position, read/write
counts, flags, descriptor, and buffer fields); native stream state follows a
reserved guest area. This is not a complete implementation of FreeBSD's private
FILE operations, locking fields, or wide-stream state.

Added character input/output, __srget/__swbuf fallbacks, fgets, ungetc, EOF/error
queries, clearerr, fileno, and buffering-mode translation. Existing fread/fwrite
and fseek now refresh the guest status fields. Tests exercise stream globals,
field offsets, inline-macro fallback paths, temporary-file I/O, EOF, ungetc,
and closure. All fourteen suites pass and the compatibility libraries rebuild.

Latest startup resolves the stream globals and stops at `8nY19bKoiZk` (`fcntl`).
The game has not reached its entry point or displayed a frame.
