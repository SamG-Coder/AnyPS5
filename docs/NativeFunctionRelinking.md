# Offline native function relinking

`--native-functions manifest.txt` rewrites audited x86-64 function entries in
an input ELF into direct jumps to ordinary native library symbols. Rewriting
happens before the native ELF/PE is written. The output contains no new opcode
interpreter, runtime code generator or command replay engine for these bindings.

This is an explicit porting primitive. It does **not** automatically port an
arbitrary AGC renderer, prove that command-buffer aliases have been eliminated,
or turn the experimental native AGC backend into a validated Quake II port.
The original application's calling convention and data contracts must match
those of each replacement. In particular, Windows replacements for PS5 System V
callers need the project's ABI adapter; a raw Microsoft-ABI function is not
interchangeable.

## Manifest

Each non-comment line contains four whitespace-separated fields:

```text
# source virtual address   native symbol       native library     exact source prologue
0x240                      native_fixture_add  native_fixture.so  b807000000c3
```

The address is an **input virtual address**, not a file offset. The final field
is 6–64 bytes of audited original instructions in hexadecimal. It must match
exactly and end on an instruction boundary. The example belongs to the test
fixture, not Quake II. Library names must be basenames, not paths.

```sh
relinker --native-functions manifest.txt --rpath /path/to/native-libraries input.elf output.elf
relinker --windows --native-functions manifest.txt input.elf output.exe
```

The relinker allocates native import slots, adds native dynamic symbols and
loader relocations, and replaces each entry with `jmp qword ptr [rip+disp32]`.
Remaining bytes of the verified prefix become NOPs. Source call sites and
function pointers that address that entry reach the native implementation
without instruction interpretation. The replacement executes with the existing
arguments and returns to the original caller.

## Validation and limits

The pass rejects mismatching bytes, out-of-range entries, truncated instructions,
overlapping replacements, loader relocations overlapping a replaced prefix,
ambiguous providers within the manifest and import slots outside RIP-relative
reach. It builds changes transactionally; a failure does not publish a partly
rewritten executable. The existing output file remains untouched on validation
failure.

A profile author must still verify that the address is an actual function entry,
that no control-flow edge enters the middle of the replaced prefix, and that no
self-modifying code rewrites it. Exact prefix comparison is a version guard, not
a whole-program alias or control-flow proof. Unknown AGC imports continue to
fail the branch's native-lowering check rather than gain an interpreter fallback.

The Linux output builder now appends a correctly sized, mapped program-header
table instead of requiring spare slots in the input. LOAD records are sorted by
virtual address, PHDR/INTERP records are emitted correctly, and a returning source
entry exits through native Linux startup code instead of executing UD2.

## Executed regression

`native_function_relink` creates an ELF fixture and a native C++ shared library,
relinks the fixture and **executes the resulting native binaries**:

- Original function returns 7.
- Rebound native function receives the original argument 19, adds 23 and returns 42.
- Malformed/mismatching manifests leave an existing output unchanged.
- The same transformation produces a PE image; Windows execution is not covered
  by this Linux test.

`native_build_contract` separately loads the native compatibility libraries with
immediate symbol resolution and rejects linked PM4 execution/state-decoder symbols.
That audit does not claim the runtime shader compiler has been moved offline.
