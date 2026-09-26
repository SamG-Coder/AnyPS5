# Native relinking branch: verified status

Snapshot: 26 September 2026. Branch: `relink/native-agc-only`.

## What has actually been executed

The native configuration builds the relinker, `libSceAgcDriver`, `libSceAgc`,
`libSceVideoOut` and the registered test suite on Linux/GCC 14. The current local
suite has **70 passing tests**. The preceding offline-rebinding commit
`58ba73f76f9de567081a0cef9c5cbb348fc3fcf1` also passed the independent GitHub Actions
build, test and linked-symbol audit. Later commits must pass their own CI runs;
this snapshot is not a substitute for the status attached to a particular SHA.

The `native_function_relink` test builds a real ELF fixture and a native C++
shared library, emits a relinked ELF and executes both binaries. The original
function returns 7; its native replacement receives argument 19 and returns 42.
No runtime instruction decoder, PM4 parser, command replay engine or JIT is used
by this test. PE output generation is tested, but Windows execution is not.

The `native_build_contract` test loads the native PRX libraries with immediate
symbol resolution. It also inspects the driver binary and rejects linked PM4
execution and legacy graphics/depth state-decoder symbols. Unit tests for legacy
decoders may still exist in separate test executables; they are not linked into
the native driver.

Additional regressions cover Linux native C++ exception handling, native function
manifest validation and transactional output, qualified import/NID resolution,
nonzero-address call-site indexing, per-stage shader argument separation, and
asynchronous failure propagation with re-entrant output callbacks.

## Quake II workload: not running yet

The pinned public test input is `blackbearreloaded/ps5-yamagi-quake2`
`v0.2.0-alpha.1`, `PPSA99007.zip`. Its SHA-256 is
`4fa5da07498fe64ffc67229eed2347e0ddffd5073f1fea248ba5e69307229a70`.
The Actions input-audit workflow records this provenance. No commercial game PAKs
or protected executable decryption are included in this branch.

The locally inspected uncompressed homebrew ELF contains 382 relocation/import
references. Unlike the earlier broken importer, the relinker now associates
qualified NIDs with their real library metadata. The native-only run currently
stops at:

```text
FAIL: AGC import has no native lowering: kW3GLb7QfPg#H#I
```

That import is `sceAgcInit`. Other workload imports still requiring coverage
include indirect context/shader/user-configuration writes, shader linking,
register defaults, release-memory operations, rendering waits, flips and suspend.
There is **no Quake gameplay result or measured FPS improvement** from this branch.

## Remaining requirements for a native port

1. Preserve the application's command-buffer ABI, including returned pointers,
   direct writes, patch APIs, allocation callbacks, subrange submission and buffer
   reuse. The experimental one-DWORD token implementation is not equivalent to
   that ABI and must not be described as a completed lowering. Offline analysis
   or an audited whole-function replacement must account for these uses.
2. Complete the native renderer binding and resource/lifetime contracts. The
   experimental AGC backend still lacks full context/API coverage, native draw
   modifiers, safe ownership/reuse, and a validated headless-to-window transition.
   Unknown settings must not be silently ignored or replaced with plausible values.
3. Move discoverable shader translation and specialization into the offline
   compiler/output pipeline. `NativeDrawCompiler` still calls the runtime shader
   recompiler; excluding `Pm4.cpp` does not make this fully ahead-of-time.
4. Prove control-flow and pointer/alias coverage before claiming an arbitrary
   executable has been automatically ported. `--native-functions` is a tested,
   explicit native binding primitive; it does not supply those proofs by itself.
5. Execute the actual workload, validate pixels, then measure CPU/GPU timing.
   Passing unit tests and having a Vulkan call in source code are not gameplay.

## Build and reproduce

```sh
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++-14 \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Native mode is the default on this branch. `AGC_NATIVE_RELINKED_ONLY=OFF` is an
explicit legacy-comparison configuration and does not meet the native-only goal.
The source depends on the repository's recursively checked-out submodules.
The named CI workflow installs its host display/Vulkan build dependencies.

This branch is a tested development baseline and an offline code-rewriting tool,
not a release-ready replacement for the existing game-running path.
