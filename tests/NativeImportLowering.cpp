#include <relinker/analysis/ImportLibraries.hpp>
#include <relinker/analysis/AgcImportLowering.hpp>
#include <relinker/analysis/AgcLoweringAnalyzer.hpp>
#include <relinker/analysis/CallSiteResolver.hpp>
#include <nid/NidCompute.hpp>
#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace {
void check(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
template<class F> void rejects(F action) {
    try { action(); } catch (const std::exception&) { return; }
    throw std::runtime_error("invalid input was accepted");
}

void libraries() {
    const std::string data = std::string("\0libSceAgc\0libSceAgcDriver\0", 27);
    const auto strings = std::span(reinterpret_cast<const std::uint8_t*>(data.data()), data.size());
    const std::array<Relinker::DynamicTag, 4> tags{{
        {0x61000049, (7ull << 48) | 1}, {0x61000045, (8ull << 48) | 1},
        {0x61000015, (8ull << 48) | 11}, {0x6100000f, (9ull << 48) | 11}
    }};
    const Relinker::ImportLibraries imports(tags, strings);
    check(imports.LibraryFor("abc#H#I") == "libSceAgc", "PS5 import library ID lost");
    check(imports.LibraryFor("abc#I#J") == "libSceAgcDriver", "legacy import tag lost");
    check(imports.LibraryFor("plain").empty(), "unqualified host symbol assigned a library");
    rejects([&] { imports.LibraryFor("abc##I"); });
    rejects([&] { imports.LibraryFor("abc#H#I#J"); });
    rejects([&] { imports.LibraryFor("abc#/#I"); });
    rejects([&] { imports.LibraryFor("abc#zzzz#I"); });
    rejects([&] { imports.LibraryFor("abc#H#Z"); });
    auto badTags = tags; badTags[0].Value = (7ull << 48) | 999;
    rejects([&] { Relinker::ImportLibraries bad(badTags, strings); });
    badTags = tags; badTags[2] = {0x61000049, (7ull << 48) | 11};
    rejects([&] { Relinker::ImportLibraries bad(badTags, strings); });
    const auto hashed = Nid::ComputeNid("sceAgcDcbDrawIndex", "libSceAgc");
    check(Relinker::AgcImportLowering::NativeSymbol(hashed + "#H#I", "libSceAgc") == "aps5NativeAgcDrawIndex", "qualified hashed import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("sceAgcCreateShader", "libSceAgc") == "aps5NativeAgcCreateShader", "named import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("wr23dPKyWc0#H#I", "libSceAgc") == "aps5NativeAgcReleaseMem", "Quake completion import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("kW3GLb7QfPg#H#I", "libSceAgc") == "aps5NativeAgcInit", "Quake initialization import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("h9z6+0hEydk#H#I", "libSceAgc") == "aps5NativeAgcSuspendPoint", "Quake suspend import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("MqAdbRMdNz4#H#I", "libSceAgc") == "aps5NativeAgcLinkShaders", "Quake shader-link import did not lower");
    check(Relinker::AgcImportLowering::NativeSymbol("Wi82ArQtAwg#H#I", "libSceAgc") == "aps5NativeAgcGetRegisterDefaults", "Quake register-defaults import did not lower");
    std::vector<Relinker::NidReference> references{{hashed + "#H#I", "libSceAgc", 7, 10, 20, 0}};
    Relinker::AgcImportLowering::Apply(references);
    check(references[0].Nid == "aps5NativeAgcDrawIndex", "dynamic import not rewritten");
    check(references[0].RelocationAddress == 20, "lowering moved source GOT slot");
}

void writeRel(std::vector<std::uint8_t>& bytes, std::size_t field, std::uint64_t end, std::uint64_t target) {
    const auto displacement = static_cast<std::int32_t>(target - end);
    std::memcpy(bytes.data() + field, &displacement, 4);
}
void callSites() {
    constexpr std::uint64_t base = 0x400000, got = 0x402000;
    std::vector<std::uint8_t> bytes(96, 0x90);
    bytes[0] = 0xff; bytes[1] = 0x15; writeRel(bytes, 2, base + 6, got);
    bytes[16] = 0xff; bytes[17] = 0x15; writeRel(bytes, 18, base + 22, got);
    // Plain import-address load and INC must not be reported as calls.
    bytes[32] = 0x48; bytes[33] = 0x8b; bytes[34] = 0x05; writeRel(bytes, 35, base + 39, got);
    bytes[40] = 0xff; bytes[41] = 0x05; writeRel(bytes, 42, base + 46, got);
    bytes[48] = 0xe8; writeRel(bytes, 49, base + 53, base + 64);
    bytes[64] = 0xff; bytes[65] = 0x25; writeRel(bytes, 66, base + 70, got);
    const auto resolver = Relinker::MakeCallSiteResolver();
    const auto sites = resolver->ResolveCallSites(bytes, base, got, 8);
    check(sites == std::vector<std::uint64_t>{base, base + 16, base + 48, base + 64}, "nonzero-VA call/PLT index wrong");
    std::fill(bytes.begin(), bytes.begin() + 6, 0x90);
    const auto changed = resolver->ResolveCallSites(bytes, base, got, 8);
    check(changed.size() == 3 && changed.front() == base + 16, "same-vector mutation left stale call index");
    check(resolver->ResolveCallSites(bytes, base + 0x1000, got, 8).empty(), "load address not part of call index identity");
    rejects([&] { resolver->ResolveCallSites(bytes, UINT64_MAX - 5, got, 8); });
    rejects([&] { resolver->ResolveCallSites(bytes, base, UINT64_MAX - 1, 8); });
}

void coverage() {
    std::vector<Relinker::CallRegistryEntry> entries(3);
    entries[0].Nid = "sceAgcInit";
    entries[1].Nid = "missingFirst";
    entries[2].Nid = "missingSecond";
    for (auto& entry : entries) {
        entry.Library = "libSceAgc";
        entry.CallSites = {16};
        entry.CallSitesResolved = true;
    }
    entries[2].CallSitesResolved = false;
    try {
        Relinker::AgcLoweringAnalyzer().Analyze(entries);
        throw std::runtime_error("incomplete native coverage accepted");
    } catch (const Relinker::RelinkerException& error) {
        const std::string message = error.what();
        check(message.find("missingFirst") != std::string::npos && message.find("missingSecond") != std::string::npos,
              "coverage report stopped at the first unsupported import");
        check(message.find("indirect or unresolved") != std::string::npos, "unresolved call sites were not reported");
        check(message.find("sceAgcInit") == std::string::npos, "supported import reported as unsupported");
    }
    entries.resize(1);
    check(Relinker::AgcLoweringAnalyzer().Analyze(entries).size() == 1, "supported native import rejected");
}
}
int main() {
    try { libraries(); callSites(); coverage(); std::cout << "Native import lowering and call-site tests passed\n"; }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
