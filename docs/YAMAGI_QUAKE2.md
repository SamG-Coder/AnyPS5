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

Observed startup result:

- Loads libSceSystemService, libSceUserService, libScePad, libSceAudioOut,
  libSceVideoOut, libSceKeyboard, libSceImeDialog, libSceAgc, libSceAgcDriver,
  and libSceLibcInternal compatibility modules successfully.
- Stops loading `libScePosixForWebKit.prx`, which AnyPS5 does not currently build.
- Exits with `0xc0000135`; no transfer to the guest entry point and no game frame.

The next work is to audit the game's required POSIX imports against existing
AnyPS5 implementations before providing the missing module. Successful module
loading alone does not establish API behavior, rendering, or playability.

## Regression coverage

`ctest --test-dir build-winlibs --output-on-failure`: four suites pass. The new
`empty_tls` integration suite checks absent PE TLS for an empty guest segment,
retained TLS for a nonempty segment, rejection of malformed sizes and empty TLS
accesses, and execution of both valid generated Windows fixtures (exit 42).
