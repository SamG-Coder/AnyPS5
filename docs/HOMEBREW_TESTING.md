# Homebrew testing

## Build and test

On Windows, install CMake, Ninja, and a C++20 compiler. For compatibility libraries,
use the toolchain specified in [README.md](../README.md).

Run from the repository root with the compiler and Ninja on `PATH`:

```sh
git submodule update --init --recursive
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --target relinker strict_nid_filter_tests windows_dependency_diagnostics_tests
ctest --test-dir build --output-on-failure -R "^(strict_nid_filter|windows_dependency_diagnostics)$"
```

## PS5 homebrew projects

- [Omnispeak](https://github.com/sulix/omnispeak): [PS5 build recipe](https://github.com/ps5-payload-dev/websrv/blob/master/homebrew/Omnispeak/build.sh).
- [Crispy Doom PS5 build recipe](https://github.com/ps5-payload-dev/websrv/blob/master/homebrew/CrispyDoom/build.sh).
- [PS5 homebrew packages, websrv v0.34](https://github.com/ps5-payload-dev/websrv/releases/tag/v0.34).
- [PS5 payload SDK](https://github.com/ps5-payload-dev/sdk).
- [Freedoom game data](https://freedoom.github.io/).

## Convert an ELF executable

Keep downloaded binaries and game data outside version control. Check the engine
and game-data licenses separately.

```sh
build/core/relinker/relinker.exe --windows downloads/input.elf downloads/output.exe
```

Check conversion, startup, rendering, input, and audio separately;
conversion alone does not establish game compatibility.
