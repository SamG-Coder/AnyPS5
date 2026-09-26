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
                        'int native_fixture_add(int value) { return value + 23; }\n')
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
        impl.write_text('extern "C" int native_fixture_add(int value) { return value + 23; }\n')
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
    print("Native entry rebinding executed: original=7, native=42; no interpreter or runtime code generation")


if __name__ == "__main__":
    main()
