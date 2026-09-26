"""Load native libraries eagerly and reject legacy execution symbols."""
import ctypes
import os
from pathlib import Path
import subprocess
import sys


def main():
    paths = [Path(arg).resolve(strict=True) for arg in sys.argv[1:]]
    if len(paths) != 3:
        raise RuntimeError("expected driver, AGC and VideoOut libraries")
    symbols = subprocess.run(["nm", "-C", "--defined-only", str(paths[0])],
                             check=True, text=True, capture_output=True).stdout
    forbidden = ("AgcDriver::Pm4::Execute(", "AgcDriver::Pm4::ResolveDraw(",
                 "AgcDriver::Pm4::ResolveValidatedDraw(",
                 "AgcDriver::Graphics::DecodeState(",
                 "AgcDriver::Graphics::DecodeDepthState(")
    for symbol in forbidden:
        if symbol in symbols:
            raise AssertionError("legacy execution linked into native driver: " + symbol)
    libraries = [ctypes.CDLL(str(path), mode=os.RTLD_NOW | os.RTLD_GLOBAL) for path in paths]
    for name in ("aps5NativeAgcSubmit", "aps5NativeAgcCreateShader", "aps5NativeAgcDrawIndex"):
        getattr(libraries[0], name)
    print("Native shared libraries loaded; legacy command execution is absent")


if __name__ == "__main__":
    main()
