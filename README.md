# About

Converter for native execution of PlayStation 5 ELF binaries on Linux and Windows through binary format conversion and ABI compatibility. The relinker implementation uses only the C++20 standard library and performs deterministic transformation of executable binary.

Implementations of system prx libraries suitable for dynamic linking: [core/libs/prx](core/libs/prx)

Releases will be published after the first full successful launch of at least one game.

## Status

Execution reaches `_start`, stack unwinding and exception handling tables are built. All unimplemented functions throw std::runtime_error. `what()` is printed to stderr and the process terminates.
Audio output and video output initialization pass.
Reaches the `main` function with render loop (deep call chain, ~64KB stack frame function) - init and shader loading from disk fully complete.

A standalone [SPIR-V triangle test](core/libs/prx/libSceAgcDriver/tests/Visual.cpp) passes GPU readback checks and presents a frame through the [libSceAgcDriver](core/libs/prx/libSceAgcDriver). The first application draw passes driver state validation and reaches shader recompilation. Application shader execution remains unverified without [recompiler](core/shader/recompiler/Recompiler.cpp).

Now: `ShaderRecompiler::Recompile: ShaderInputInfoBuilder: vertex/tessellation/mesh input info is not implemented`.

## Build

The relinker uses only the C++20 standard library and should build with any conforming compiler.

[libc.prx](core/libs/prx/libc) implementations contain compiler-specific code. Linux builds work with GCC; on Windows, MinGW-w64 GCC 15.2.0 (`x86_64-ucrt-posix-seh`) is currently required.

The project targets maximum compiler portability. Support for additional compilers will be addressed after the first successful game launch.

## Disclaimer

This project is intended for interoperability, research, preservation, and compatibility purposes. It does not include, distribute, or require copyrighted software, firmware, cryptographic keys, or proprietary libraries. Users are responsible for ensuring that any binaries used with this project are obtained and used in accordance with applicable laws and their respective license terms.

## License

This project is licensed under the GNU General Public License version 2 only.
