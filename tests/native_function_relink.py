"""Emit and execute a native ELF whose function entry is rebound offline."""
from pathlib import Path
import struct
import subprocess
import sys
import tempfile


def image():
    data = bytearray(4096)
    data[:16] = b"\x7fELF\x02\x01\x01" + bytes(9)
    struct.pack_into("<HHIQQQIHHHHHH", data, 16,
                     3, 62, 1, 0x200, 64, 0, 0, 64, 56, 2, 64, 0, 0)
    # Native source entry: align stack, call a local function with argument 19.
    data[0x200:0x20d] = b"\x55\xbf\x13\x00\x00\x00\xe8\x35\x00\x00\x00\x5d\xc3"
    data[0x240:0x246] = b"\xb8\x07\x00\x00\x00\xc3"  # Original implementation returns 7.
    tags = [(5, 0x600), (10, 1), (6, 0x620), (11, 24),
            (7, 0x700), (8, 0), (9, 24), (0, 0)]
    struct.pack_into("<IIQQQQQQ", data, 64, 1, 7, 0, 0, 0, len(data), len(data), 4096)
    struct.pack_into("<IIQQQQQQ", data, 120, 2, 6, 0x400, 0x400, 0x400,
                     len(tags) * 16, len(tags) * 16, 8)
    for i, tag in enumerate(tags):
        struct.pack_into("<qQ", data, 0x400 + i * 16, *tag)
    return data


def strict_image():
    data = bytearray(0x3000)
    data[:16] = b"\x7fELF\x02\x01\x01" + bytes(9)
    struct.pack_into("<HHIQQQIHHHHHH", data, 16,
                     3, 62, 1, 0x1000, 64, 0, 0, 64, 56, 5, 64, 0, 0)
    data[0x1000:0x1080] = b"\xcc" * 0x80
    data[0x1000:0x100d] = b"\x55\xbf\x13\x00\x00\x00\xe8\x35\x00\x00\x00\x5d\xc3"
    data[0x1040:0x1047] = b"\xff\x15" + struct.pack("<i", 0x2800 - 0x1046) + b"\xc3"
    strings = b"\0unknownNative#A#A\0libSceAgc\0"
    library = strings.index(b"libSceAgc")
    data[0x2600:0x2600 + len(strings)] = strings
    struct.pack_into("<IBBHQQ", data, 0x2698, 1, 0x12, 0, 0, 0, 0)
    struct.pack_into("<II", data, 0x26c0, 1, 2)
    struct.pack_into("<QQq", data, 0x2700, 0x2800, (1 << 32) | 6, 0)
    tags = [(5, 0x2600), (10, len(strings)), (6, 0x2680), (11, 24),
            (0x6100002f, 0x2700), (0x61000031, 24), (0x61000033, 24), (4, 0x26c0),
            (0x61000015, library), (0x6100000f, library), (0, 0)]
    headers = [
        (1, 5, 0x1000, 0x1000, 0x1000, 0x80, 0x80, 4096),
        (1, 6, 0x2000, 0x2000, 0x2000, 0x700, 0x700, 4096),
        (1, 6, 0x2800, 0x2800, 0x2800, 0x800, 0x800, 8),
        (2, 6, 0x2400, 0x2400, 0x2400, len(tags) * 16, len(tags) * 16, 8),
        (0x6474e550, 4, 0x2900, 0x2900, 0x2900, 32, 32, 8),
    ]
    for i, header in enumerate(headers):
        struct.pack_into("<IIQQQQQQ", data, 64 + i * 56, *header)
    for i, tag in enumerate(tags):
        struct.pack_into("<qQ", data, 0x2400 + i * 16, *tag)
    struct.pack_into("<BBBBQIQQ", data, 0x2900, 1, 0, 3, 0, 0x2920, 1, 0x1040, 0x2940)
    struct.pack_into("<II5B", data, 0x2920, 9, 0, 1, 0, 1, 0x78, 16)
    struct.pack_into("<IIQQ", data, 0x2940, 20, 0x24, 0x1040, 0x10)
    return data


def strict_replacement(relinker, work, library, windows):
    source = work / "strict.elf"
    original = strict_image()
    source.write_bytes(original)
    manifest = work / "strict-bindings.txt"
    prefix = original[0x1040:0x1046].hex()
    manifest.write_text(f"0x1040 native_fixture_add {library} {prefix}\n")
    output = work / ("strict.exe" if windows else "strict")
    platform = ["--windows"] if windows else []
    base = [relinker, *platform, "unused-filter=2", "--rpath", work]
    result = run([*base, source, output])
    if result.returncode != 2 or "AGC import has no native lowering" not in result.stderr:
        raise AssertionError(("live AGC import accepted", result.stdout, result.stderr))
    result = run([*base, "--native-functions", manifest, source, output])
    if result.returncode or "Strict filtering total: 1 -> 0" not in result.stdout:
        raise AssertionError(("replacement import filtering failed", result.stdout, result.stderr))
    if not windows:
        output.chmod(0o700)
    executed = run([output], cwd=work)
    if executed.returncode != 42:
        raise AssertionError(("strict replacement execution failed", executed.returncode, executed.stderr))
    emitted = output.read_bytes()
    invalid = bytearray(original)
    invalid[0x1040] = 0x90
    variants = [(invalid, "prologue does not match")]
    invalid = bytearray(original)
    invalid[0x1006:0x100c] = b"\xff\x15" + struct.pack("<i", 0x2800 - 0x100c)
    variants.append((invalid, "AGC import has no native lowering"))
    invalid = bytearray(original)
    struct.pack_into("<Q", invalid, 0x2700, 0x1040)
    variants.append((invalid, "overlaps an original loader relocation"))
    invalid = bytearray(original)
    struct.pack_into("<Q", invalid, 0x2808, 0x1046)
    variants.append((invalid, "reachable target enters a replaced function body"))
    invalid = bytearray(original)
    struct.pack_into("<Q", invalid, 0x2950, 5)
    variants.append((invalid, "native prologue exceeds its function boundary"))
    invalid = bytearray(original)
    struct.pack_into("<I", invalid, 0x290c, 0)
    variants.append((invalid, "native replacement has no known function boundary"))
    for invalid, expected in variants:
        source.write_bytes(invalid)
        result = run([*base, "--native-functions", manifest, source, output])
        if result.returncode != 2 or expected not in result.stderr or output.read_bytes() != emitted:
            raise AssertionError(("invalid strict replacement accepted", expected, result.stdout, result.stderr))


def indirect_import(relinker, work, library, windows):
    data = strict_image()
    data[0x1000:0x1080] = b"\xcc" * 0x80
    code = bytearray(b"\x55\xbf\x13\0\0\0")
    code += b"\x48\x8b\x05" + struct.pack("<i", 0x2800 - 0x100d)
    code += b"\x48\x89\x05" + struct.pack("<i", 0x2808 - 0x1014)
    code += b"\xff\x15" + struct.pack("<i", 0x2808 - 0x101a)
    code += b"\x5d\xc3"
    data[0x1000:0x1000 + len(code)] = code
    strings = b"\0sceAgcInit#A#A\0libSceAgc\0" + library.encode() + b"\0"
    data[0x2600:0x2600 + len(strings)] = strings
    struct.pack_into("<Q", data, 0x2418, len(strings))
    for i in (8, 9):
        struct.pack_into("<Q", data, 0x2400 + i * 16 + 8, strings.index(b"libSceAgc"))
    struct.pack_into("<qQ", data, 0x24a0, 1, strings.index(library.encode()))
    struct.pack_into("<qQ", data, 0x24b0, 0, 0)
    struct.pack_into("<QQ", data, 64 + 3 * 56 + 32, 12 * 16, 12 * 16)
    source = work / "indirect.elf"
    source.write_bytes(data)
    output = work / ("indirect.exe" if windows else "indirect")
    platform = ["--windows"] if windows else []
    result = run([relinker, *platform, "unused-filter=0", "--rpath", work, source, output])
    if result.returncode or "Native AGC lowering sites: 0" not in result.stdout:
        raise AssertionError(("indirect native import rejected", result.stdout, result.stderr))
    if not windows:
        output.chmod(0o700)
    executed = run([output], cwd=work)
    if executed.returncode != 42:
        raise AssertionError(("copied native import did not execute", executed.returncode, executed.stdout, executed.stderr))
    emitted = output.read_bytes()
    source.write_bytes(data.replace(b"sceAgcInit#A#A", b"badAgcInit#A#A"))
    result = run([relinker, *platform, "unused-filter=0", "--rpath", work, source, output])
    if result.returncode != 2 or "AGC import has no native lowering" not in result.stderr or output.read_bytes() != emitted:
        raise AssertionError(("unsupported indirect import accepted", result.stdout, result.stderr))


def run(args, **kwargs):
    return subprocess.run([str(a) for a in args], capture_output=True, text=True, timeout=30, **kwargs)


def windows_main(relinker, compiler):
    with tempfile.TemporaryDirectory(prefix="native-function-pe-") as directory:
        work = Path(directory)
        source = work / "source.elf"
        source.write_bytes(image())
        manifest = work / "bindings.txt"
        valid = "0x240 native_fixture_add native_fixture.prx b807000000c3\n"
        manifest.write_text(valid)
        impl = work / "native.cpp"
        impl.write_text('extern "C" __attribute__((dllexport,sysv_abi)) '
                        'int native_fixture_add(int value) { return value + 23; }\n'
                        'extern "C" __attribute__((dllexport,sysv_abi)) int aps5NativeAgcInit(int value) { return value + 23; }\n')
        compiled = run([compiler, "-shared", impl, "-o", work / "native_fixture.prx"])
        if compiled.returncode:
            raise AssertionError((compiled.stdout, compiled.stderr))
        for name, expected, options in (
            ("original.exe", 7, []),
            ("ported.exe", 42, ["--native-functions", manifest]),
        ):
            output = work / name
            result = run([relinker, "--windows", *options, "--rpath", work, source, output])
            if result.returncode:
                raise AssertionError((result.stdout, result.stderr))
            executed = run([output], cwd=work)
            if executed.returncode != expected:
                raise AssertionError((name, executed.returncode, executed.stdout, executed.stderr))
        output = work / "ported.exe"
        emitted = output.read_bytes()
        manifest.write_text(valid.replace("b807", "b808"))
        result = run([relinker, "--windows", "--native-functions", manifest,
                      "--rpath", work, source, output])
        if result.returncode != 2 or output.read_bytes() != emitted:
            raise AssertionError(("invalid PE lowering modified output", result.stdout, result.stderr))
        strict_replacement(relinker, work, "native_fixture.prx", True)
        indirect_import(relinker, work, "native_fixture.prx", True)
    print("PE loader execution verified: original=7, native System V replacement=42")


def main():
    relinker, compiler = map(Path, sys.argv[1:3])
    if sys.platform == "win32":
        windows_main(relinker, compiler)
        return
    with tempfile.TemporaryDirectory(prefix="native-function-relink-") as directory:
        work = Path(directory)
        source = work / "source.elf"
        source.write_bytes(image())
        manifest = work / "bindings.txt"
        manifest.write_text("0x240 native_fixture_add native_fixture.so b807000000c3\n")
        impl = work / "native.cpp"
        impl.write_text('extern "C" int native_fixture_add(int value) { return value + 23; }\n'
                        'extern "C" int aps5NativeAgcInit(int value) { return value + 23; }\n')
        compiled = run([compiler, "-shared", "-fPIC", impl, "-o", work / "native_fixture.so"])
        if compiled.returncode:
            raise AssertionError(compiled.stderr)
        original = work / "original"
        result = run([relinker, source, original])
        if result.returncode:
            raise AssertionError((result.stdout, result.stderr))
        original.chmod(0o700)
        executed = run([original])
        if executed.returncode != 7:
            raise AssertionError(("original native entry", executed.returncode, executed.stderr))
        output = work / "ported"
        result = run([relinker, "--native-functions", manifest, "--rpath", work, source, output])
        if result.returncode:
            raise AssertionError((result.stdout, result.stderr))
        output.chmod(0o700)
        emitted = output.read_bytes()
        if emitted[0x240:0x242] != b"\xff\x25":
            raise AssertionError("source function was not replaced with a native indirect jump")
        if b"native_fixture_add\0" not in emitted or b"native_fixture.so\0" not in emitted:
            raise AssertionError("native loader imports missing")
        # The runtime loader must see a complete mapped PHDR table, one
        # interpreter and ascending PT_LOADs even when the source had only two
        # header slots. Added imports must not overwrite source instructions.
        phoff = struct.unpack_from("<Q", emitted, 32)[0]
        phsize, phnum = struct.unpack_from("<HH", emitted, 54)
        headers = [struct.unpack_from("<IIQQQQQQ", emitted, phoff + i * phsize)
                   for i in range(phnum)]
        loads = [ph for ph in headers if ph[0] == 1]
        if [ph[3] for ph in loads] != sorted(ph[3] for ph in loads):
            raise AssertionError("native LOAD headers are not ordered by address")
        phdr = [ph for ph in headers if ph[0] == 6]
        if len(phdr) != 1 or not any(ph[3] <= phdr[0][3] and
                phdr[0][3] + phdr[0][6] <= ph[3] + ph[6] for ph in loads):
            raise AssertionError("native PHDR table is not mapped")
        if sum(ph[0] == 3 for ph in headers) != 1:
            raise AssertionError("native interpreter header missing or duplicated")
        executed = run([output])
        if executed.returncode != 42:
            raise AssertionError(("native replacement did not execute", executed.returncode, executed.stdout, executed.stderr))
        # Incorrect manifests fail before touching an existing output file.
        for line in (
            "0x240 native_fixture_add native_fixture.so b808000000c3\n",
            "0x99999 native_fixture_add native_fixture.so b807000000c3\n",
            "0x240 native_fixture_add native_fixture.so 00\n",
            "0x240 native_fixture_add ../bad.so b807000000c3\n",
            "0x240 native_fixture_add native_fixture.so b807000000c3\n" * 2,
        ):
            manifest.write_text(line)
            result = run([relinker, "--native-functions", manifest, source, output])
            if result.returncode != 2 or output.read_bytes() != emitted:
                raise AssertionError(("invalid lowering was not transactional", line, result.stdout, result.stderr))
        # PE generation uses the same transformed instructions/import relocations.
        manifest.write_text("0x240 native_fixture_add native_fixture.prx b807000000c3\n")
        result = run([relinker, "--windows", "--native-functions", manifest, source, work / "ported.exe"])
        if result.returncode or (work / "ported.exe").read_bytes()[:2] != b"MZ":
            raise AssertionError(("PE native import lowering", result.stdout, result.stderr))
        strict_replacement(relinker, work, "native_fixture.so", False)
        indirect_import(relinker, work, "native_fixture.so", False)
    print("Native entry rebinding executed: original=7, native=42; no interpreter or runtime code generation")


if __name__ == "__main__":
    main()
