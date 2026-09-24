# Homebrew investigation, 2026-09-25

Baseline: SamG-Coder/AnyPS5, commit `2778c1e`.

## Verified locally

- Cloned the fork and initialized all seven submodules at their pinned revisions.
- Configured on Windows with CMake/Ninja and MSYS2 UCRT64 GCC 16.2.0.
- Built `relinker`, `strict_nid_filter_tests`, and `windows_dependency_diagnostics_tests`.
- Both existing test executables passed. Initially CTest discovered zero tests.
- Enabled root CTest registration when `BUILD_TESTING=ON` and registered the Windows diagnostics test. The latter is included in normal builds only when testing is enabled.
- After the CMake changes, CTest discovered and passed both tests.
- Tried relinking two real PS5 homebrew binaries to Windows. Both failed with exit code 2; neither produced a game executable.

This verifies the relinker build and these two test suites only. The compatibility libraries, shader pipeline, and game execution have not been built or validated in this investigation. Upstream documents a specific WinLibs GCC 15.2.0 toolchain requirement for its Windows libc implementation; the compiler used here does not establish compatibility with that requirement.

## Reproduce the build

Run from the repository root in PowerShell. These paths match this workstation:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
cmake -S . -B build -G Ninja `
  -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe `
  -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe `
  -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --target relinker strict_nid_filter_tests windows_dependency_diagnostics_tests -j 8
ctest --test-dir build --output-on-failure
```

## Public PS5 homebrew inputs

Downloaded directly from [ps5-payload-dev/websrv v0.34](https://github.com/ps5-payload-dev/websrv/releases/tag/v0.34). Downloads remain in the ignored `downloads/` directory; no game binaries or data are added to Git.

| Archive | SHA-256 |
| --- | --- |
| Omnispeak.zip | 7A4D82BF58EF7406A4C9050EC298195C548D387FEBC28FD814C3BF9026BB231B |
| CrispyDoom.zip | C0C37713703E109F43B1B598CF5790D969097A2BAF961693A0FD0E03956A3C59 |

[Omnispeak](https://github.com/sulix/omnispeak) is an open-source Commander Keen engine. Its upstream README links Keen 4 shareware data, and the PS5 package includes CK4 data. The [PS5 build recipe](https://github.com/ps5-payload-dev/websrv/blob/master/homebrew/Omnispeak/build.sh) identifies how that package is assembled.

[Crispy Doom's PS5 recipe](https://github.com/ps5-payload-dev/websrv/blob/master/homebrew/CrispyDoom/build.sh) builds the engine and packages shareware data. [Freedoom](https://freedoom.github.io/) is a separate free-content candidate for later compatibility testing; it has not been downloaded or tested here.

These PS5 builds are ELF payloads, not ISO disc images. An engine's open-source license does not automatically apply to original commercial game data.

```powershell
.\build\core\relinker\relinker.exe --windows `
  downloads\Omnispeak\omnispeak.elf downloads\omnispeak.exe
.\build\core\relinker\relinker.exe --windows `
  downloads\CrispyDoom\CrispyDoom\crispy-doom.elf downloads\crispy-doom.exe
```

Both currently report:

```text
System: Windows; unused-filter=0
FAIL: Neither DT_OS_ nor DT_ variant present for DT_PLTGOT
```

## Why this is more than a missing table

`RelinkerPipeline.cpp` unconditionally requires PLTGOT, PLTRELSZ, PLTREL, and JMPREL metadata. Inspection of Omnispeak with objdump shows ordinary RELA relocations but no PLTGOT/PLT relocation tags. Supporting ELF files without PLT relocations is a candidate improvement, but simply bypassing the requirement does not establish a runnable conversion.

The [PS5 payload SDK startup](https://github.com/ps5-payload-dev/sdk/blob/master/crt/crt.c) takes a `payload_args_t` pointer and initializes console-specific kernel and dynamic-linker facilities. Its [entry assembly](https://github.com/ps5-payload-dev/sdk/blob/master/crt/_start.S) calls that startup. Homebrew payload startup therefore needs separate compatibility analysis and likely an adapter.

Omnispeak imports `libkernel_web.sprx` and `libSceLibcInternal.sprx`, among other libraries. In this AnyPS5 revision, the former contains only `APS5_DUMMY_FUN`, while the latter exposes only a heap tracing stub. File presence is not evidence of implemented compatibility.

## Suggested next milestones

1. Create a tiny source-owned executable fixture with a documented entry ABI, one imported function, and a verifiable exit value. Relink it and execute the result on Windows.
2. Add regression coverage for ELF files without PLT relocations before changing their handling.
3. Report unsupported payload startup and missing imports explicitly, then implement the minimum required compatibility path.
4. Advance to a framebuffer demo, input, and audio before retrying a complete homebrew game.

Keep conversion, loader startup, first rendered frame, and playable gameplay as distinct results. A native Windows build of the same open-source engine is useful as a reference but is not evidence that PS5 binary conversion works.
