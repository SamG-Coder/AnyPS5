#include <Cli.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Cli {

Args ParseArgs(int argc, char* argv[]) {
    Args args;
    bool unusedFilterSpecified = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--skip-syscall-check") {
            args.skipSyscallCheck = true;
        } else if (arg == "--to-intel") {
            args.toIntel = true;
        } else if (arg.rfind("unused-filter=", 0) == 0) {
            const std::string value = arg.substr(14);
            if (unusedFilterSpecified || value.size() != 1 || value[0] < '0' || value[0] > '2')
                throw std::runtime_error("unused-filter must be specified once with a value of 0, 1 or 2");
            args.unusedFilterLevel = static_cast<std::uint32_t>(value[0] - '0');
            unusedFilterSpecified = true;
        } else if (arg == "--registry") {
            args.writeRegistry = true;
        } else if (arg == "--native-functions") {
            if (!args.nativeFunctionsPath.empty() || i + 1 >= argc)
                throw std::runtime_error("--native-functions requires one manifest path");
            args.nativeFunctionsPath = argv[++i];
        } else if (arg == "--rpath") {
            if (i + 1 >= argc)
                throw std::runtime_error("--rpath requires a value");
            args.runPath = argv[++i];
        } else if (arg == "--windows") {
            args.toWindows = true;
        } else if (arg == "--lazy-binding") {
            args.lazyBinding = true;
        } else if (arg == "--autorun") {
            args.autorun = true;
        } else if (arg == "--windows-diagnostics") {
            args.windowsDiagnostics = true;
        } else if (arg.rfind("--", 0) == 0 || arg == "unused-filter") {
            throw std::runtime_error("unknown option: " + arg);
        } else if (args.inputPath.empty()) {
            args.inputPath = arg;
        } else if (args.outputPath.empty()) {
            args.outputPath = arg;
        } else {
            throw std::runtime_error("unexpected argument: " + arg);
        }
    }

    if (args.toIntel && !args.nativeFunctionsPath.empty())
        throw std::runtime_error("--native-functions cannot be combined with --to-intel");

    if (args.windowsDiagnostics && !args.toWindows)
        throw std::runtime_error("--windows-diagnostics requires --windows");

    if (args.inputPath.empty() || args.outputPath.empty())
        throw std::runtime_error(
            "Usage: relinker [--windows] [--windows-diagnostics] [--skip-syscall-check] [--to-intel] [unused-filter=0|1|2] [--registry] [--native-functions <manifest>] [--rpath <path>] [--lazy-binding] [--autorun] <input.elf> <output.elf>\n"
            "Example: relinker input.elf output.elf"
        );

    return args;
}

}
