#include <relinker/lowering/NativeFunctions.hpp>
#include <codegen/x86/X64InstructionDecoder.hpp>
#include <algorithm>
#include <charconv>
#include <fstream>
#include <limits>
#include <optional>
#include <set>
#include <sstream>

namespace Relinker {
namespace {
constexpr std::uint64_t page = 4096;
constexpr std::size_t maxBindings = 4096;

std::uint64_t align(std::uint64_t value, std::uint64_t alignment) {
    if (value > UINT64_MAX - (alignment - 1)) throw RelinkerException("Native function layout overflows");
    return (value + alignment - 1) & ~(alignment - 1);
}
void append(std::vector<std::uint8_t>& bytes, std::uint64_t value, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) bytes.push_back(static_cast<std::uint8_t>(value >> (i * 8)));
}
std::uint64_t read64(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    if (offset > bytes.size() || 8 > bytes.size() - offset) throw RelinkerException("Truncated native import metadata");
    std::uint64_t result = 0;
    for (unsigned i = 0; i < 8; ++i) result |= static_cast<std::uint64_t>(bytes[offset + i]) << (8 * i);
    return result;
}
std::string readString(const std::vector<std::uint8_t>& bytes, std::uint64_t offset) {
    if (offset >= bytes.size()) throw RelinkerException("Native import string is outside the string table");
    const auto begin = bytes.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto end = std::find(begin, bytes.end(), 0);
    if (end == bytes.end()) throw RelinkerException("Native import string is not terminated");
    return {begin, end};
}
std::uint32_t appendString(std::vector<std::uint8_t>& bytes, const std::string& value) {
    if (bytes.size() > UINT32_MAX || value.size() + 1 > UINT32_MAX - bytes.size())
        throw RelinkerException("Native import string table exceeds ELF32 string offsets");
    const auto offset = static_cast<std::uint32_t>(bytes.size());
    bytes.insert(bytes.end(), value.begin(), value.end());
    bytes.push_back(0);
    return offset;
}
void validateName(const std::string& name, bool library) {
    if (name.empty() || name.size() > 1024) throw RelinkerException("Invalid native function symbol/library name");
    for (const unsigned char ch : name)
        if (ch <= 32 || ch >= 127 || ch == '#' || (library && (ch == '/' || ch == '\\' || ch == ':')))
            throw RelinkerException("Invalid native function symbol/library name: " + name);
}
void addImport(SysVDynamicSection& dynamic, const NativeFunctionBinding& binding, VirtualAddress slot) {
    if (dynamic.DynSymData.size() % 24 || dynamic.RelaData.size() % 24 || dynamic.DynamicSegmentData.size() % 16)
        throw RelinkerException("Malformed dynamic metadata during native lowering");
    bool libraryFound = false;
    for (std::size_t offset = 0; offset < dynamic.DynamicSegmentData.size(); offset += 16) {
        if (read64(dynamic.DynamicSegmentData, offset) != 1) throw RelinkerException("Unexpected native dependency tag");
        if (readString(dynamic.DynStrData, read64(dynamic.DynamicSegmentData, offset + 8)) == binding.library)
            libraryFound = true;
    }
    if (!libraryFound) {
        append(dynamic.DynamicSegmentData, 1, 8);
        append(dynamic.DynamicSegmentData, appendString(dynamic.DynStrData, binding.library), 8);
    }
    const auto symbolIndex = dynamic.DynSymData.size() / 24;
    if (symbolIndex > UINT32_MAX) throw RelinkerException("Too many native dynamic symbols");
    append(dynamic.DynSymData, appendString(dynamic.DynStrData, binding.symbol), 4);
    append(dynamic.DynSymData, 0x12, 1); // STB_GLOBAL | STT_FUNC
    append(dynamic.DynSymData, 0, 3);    // STV_DEFAULT, SHN_UNDEF
    append(dynamic.DynSymData, 0, 8);
    append(dynamic.DynSymData, 0, 8);
    append(dynamic.RelaData, slot, 8);
    append(dynamic.RelaData, (static_cast<std::uint64_t>(symbolIndex) << 32) | 6u, 8); // GLOB_DAT
    append(dynamic.RelaData, 0, 8);
}
FileByteOffset locate(const std::vector<std::uint8_t>& bytes, const std::vector<ProgramHeader>& headers,
                      const NativeFunctionBinding& binding) {
    std::optional<FileByteOffset> found;
    for (const auto& header : headers) {
        if (header.Type != 1 || (header.Flags & 1) == 0 || binding.address < header.MappedAddress) continue;
        const auto offset = binding.address - header.MappedAddress;
        if (offset >= header.FileSize || binding.expected.size() > header.FileSize - offset) continue;
        if (found) throw RelinkerException("Native function overlaps multiple executable segments", binding.address);
        if (header.Offset > bytes.size() || header.FileSize > bytes.size() - header.Offset)
            throw RelinkerException("Native function segment exceeds source bytes", header.Offset);
        found = header.Offset + offset;
    }
    if (!found) throw RelinkerException("Native function is not in file-backed executable memory", binding.address);
    return *found;
}
void rejectInteriorBranches(const std::vector<std::uint8_t>& bytes,
                            const std::vector<ProgramHeader>& headers,
                            std::span<const NativeFunctionBinding> bindings) {
    const auto replaced = [&](VirtualAddress address) {
        return std::any_of(bindings.begin(), bindings.end(), [&](const auto& binding) {
            return address >= binding.address && address - binding.address < binding.expected.size();
        });
    };
    Codegen::X64InstructionDecoder decoder;
    for (const auto& header : headers) {
        if (header.Type != 1 || (header.Flags & 1) == 0) continue;
        if (header.Offset > bytes.size() || header.FileSize > bytes.size() - header.Offset ||
            header.FileSize > UINT64_MAX - header.MappedAddress)
            throw RelinkerException("Invalid executable segment during native branch verification");
        for (std::uint64_t offset = 0; offset < header.FileSize;) {
            const auto* instruction = bytes.data() + header.Offset + offset;
            const auto length = decoder.Decode(instruction, header.FileSize - offset);
            if (!length || length > header.FileSize - offset)
                throw RelinkerException("Invalid instruction during native branch verification", header.MappedAddress + offset);
            const auto address = header.MappedAddress + offset;
            offset += length;
            if (replaced(address)) continue;
            std::size_t prefix = 0;
            while (prefix < length) {
                const auto value = instruction[prefix];
                if ((value >= 0x40 && value <= 0x4f) || value == 0x66 || value == 0x67 ||
                    value == 0xf2 || value == 0xf3 || value == 0x2e || value == 0x3e ||
                    value == 0x26 || value == 0x36 || value == 0x64 || value == 0x65) ++prefix;
                else break;
            }
            if (prefix == length) continue;
            const auto op = instruction[prefix];
            std::optional<std::int64_t> displacement;
            if ((op == 0xe8 || op == 0xe9) && length - prefix == 5) {
                const auto field = prefix + 1;
                const auto value = static_cast<std::uint32_t>(instruction[field]) |
                    (static_cast<std::uint32_t>(instruction[field + 1]) << 8) |
                    (static_cast<std::uint32_t>(instruction[field + 2]) << 16) |
                    (static_cast<std::uint32_t>(instruction[field + 3]) << 24);
                displacement = static_cast<std::int32_t>(value);
            } else if ((op == 0xeb || (op >= 0x70 && op <= 0x7f) || (op >= 0xe0 && op <= 0xe3)) && length - prefix == 2) {
                displacement = static_cast<std::int8_t>(instruction[prefix + 1]);
            } else if (op == 0x0f && length - prefix == 6 && instruction[prefix + 1] >= 0x80 && instruction[prefix + 1] <= 0x8f) {
                const auto field = prefix + 2;
                const auto value = static_cast<std::uint32_t>(instruction[field]) |
                    (static_cast<std::uint32_t>(instruction[field + 1]) << 8) |
                    (static_cast<std::uint32_t>(instruction[field + 2]) << 16) |
                    (static_cast<std::uint32_t>(instruction[field + 3]) << 24);
                displacement = static_cast<std::int32_t>(value);
            }
            if (!displacement) continue;
            const auto end = header.MappedAddress + offset;
            VirtualAddress target;
            if (*displacement < 0) {
                const auto distance = static_cast<std::uint64_t>(-*displacement);
                if (distance > end) continue;
                target = end - distance;
            } else {
                if (static_cast<std::uint64_t>(*displacement) > UINT64_MAX - end) continue;
                target = end + *displacement;
            }
            for (const auto& binding : bindings)
                if (target > binding.address && target - binding.address < binding.expected.size())
                    throw RelinkerException("Direct branch enters the interior of a native replacement", address);
        }
    }
}
void rejectRelocationOverlap(const std::vector<std::uint8_t>& relocations, const NativeFunctionBinding& binding) {
    if (relocations.size() % 24) throw RelinkerException("Malformed native relocation table");
    for (std::size_t offset = 0; offset < relocations.size(); offset += 24) {
        const auto address = read64(relocations, offset);
        if ((address >= binding.address && address - binding.address < binding.expected.size()) ||
            (address < binding.address && binding.address - address < 8))
            throw RelinkerException("Native function prologue overlaps a loader relocation", binding.address);
    }
}
}

std::vector<NativeFunctionBinding> ReadNativeFunctionBindings(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) throw RelinkerException("Cannot read native function manifest: " + path.string());
    std::vector<NativeFunctionBinding> result;
    std::string line;
    unsigned lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        const auto first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos || line[first] == '#') continue;
        std::istringstream fields(line);
        NativeFunctionBinding binding{};
        std::string address, expected, extra;
        if (!(fields >> address >> binding.symbol >> binding.library >> expected) || fields >> extra)
            throw RelinkerException("Expected address, symbol, library and prologue hex at native manifest line " + std::to_string(lineNumber));
        const auto digits = std::string_view(address).substr(address.starts_with("0x") ? 2 : 0);
        const auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), binding.address, 16);
        if (digits.empty() || parsed.ec != std::errc{} || parsed.ptr != digits.data() + digits.size())
            throw RelinkerException("Invalid native function address at line " + std::to_string(lineNumber));
        if (expected.size() < 12 || expected.size() > 128 || expected.size() % 2)
            throw RelinkerException("Native function prologue must contain 6 to 64 complete instruction bytes");
        for (std::size_t i = 0; i < expected.size(); i += 2) {
            unsigned byte;
            const auto value = std::from_chars(expected.data() + i, expected.data() + i + 2, byte, 16);
            if (value.ec != std::errc{} || value.ptr != expected.data() + i + 2)
                throw RelinkerException("Invalid native prologue hex at line " + std::to_string(lineNumber));
            binding.expected.push_back(static_cast<std::uint8_t>(byte));
        }
        validateName(binding.symbol, false);
        validateName(binding.library, true);
        result.push_back(std::move(binding));
        if (result.size() > maxBindings) throw RelinkerException("Too many native function bindings");
    }
    if (!input.eof()) throw RelinkerException("Failed reading native function manifest");
    if (result.empty()) throw RelinkerException("Native function manifest is empty");
    return result;
}

void LowerNativeFunctions(std::vector<std::uint8_t>& source, RelinkResult& result,
                          std::span<const NativeFunctionBinding> bindings) {
    if (bindings.empty()) return;
    if (bindings.size() > maxBindings) throw RelinkerException("Too many native function bindings");
    std::uint64_t last = 0;
    for (const auto& header : result.OriginalHeaders) {
        if (header.Type != 1) continue;
        if (header.MemorySize > UINT64_MAX - header.MappedAddress) throw RelinkerException("Native load address overflow");
        last = std::max(last, header.MappedAddress + header.MemorySize);
    }
    if (last == 0) throw RelinkerException("Native lowering requires a loadable image");
    const auto nativeBase = align(last, page), nativeOffset = align(source.size(), page);
    const auto nativeBytes = bindings.size() * 8u;
    if (nativeBytes > SIZE_MAX - nativeOffset || align(nativeBytes, page) > UINT64_MAX - nativeBase)
        throw RelinkerException("Native image size overflow");
    rejectInteriorBranches(source, result.OriginalHeaders, bindings);
    // Work on copies so a failed verification never produces a partially lowered image.
    auto bytes = source;
    auto dynamic = result.DynamicSection;
    auto headers = result.OriginalHeaders;
    std::vector<std::pair<VirtualAddress, std::size_t>> written;
    std::map<std::string, std::string> symbolOwners;
    Codegen::X64InstructionDecoder decoder;
    for (std::size_t index = 0; index < bindings.size(); ++index) {
        const auto& binding = bindings[index];
        validateName(binding.symbol, false);
        validateName(binding.library, true);
        if (binding.expected.size() < 6 || binding.expected.size() > 64 || binding.expected.size() > UINT64_MAX - binding.address)
            throw RelinkerException("Invalid native function prologue range", binding.address);
        for (const auto& [address, size] : written)
            if (binding.address < address + size && address < binding.address + binding.expected.size())
                throw RelinkerException("Overlapping native function replacements", binding.address);
        const auto [owner, inserted] = symbolOwners.emplace(binding.symbol, binding.library);
        if (!inserted && owner->second != binding.library) throw RelinkerException("Ambiguous native symbol provider: " + binding.symbol);
        const auto offset = locate(source, headers, binding);
        if (!std::equal(binding.expected.begin(), binding.expected.end(), source.begin() + offset))
            throw RelinkerException("Native function prologue does not match the audited source", binding.address);
        for (std::size_t cursor = 0; cursor < binding.expected.size();) {
            const auto length = decoder.Decode(binding.expected.data() + cursor, binding.expected.size() - cursor);
            if (length == 0 || length > binding.expected.size() - cursor)
                throw RelinkerException("Native prologue splits an instruction", binding.address + cursor);
            cursor += length;
        }
        rejectRelocationOverlap(dynamic.RelaData, binding);
        rejectRelocationOverlap(dynamic.RelaPltData, binding);
        const auto slot = nativeBase + index * 8u;
        const auto next = binding.address + 6;
        if (slot < next || slot - next > INT32_MAX) throw RelinkerException("Native import slot exceeds x86 RIP-relative reach", binding.address);
        const auto displacement = static_cast<std::uint32_t>(slot - next);
        std::fill_n(bytes.begin() + offset, binding.expected.size(), 0x90);
        bytes[offset] = 0xff;
        bytes[offset + 1] = 0x25; // jmp qword ptr [rip+disp32]; preserve source calling convention
        for (unsigned byte = 0; byte < 4; ++byte) bytes[offset + 2 + byte] = static_cast<std::uint8_t>(displacement >> (byte * 8));
        addImport(dynamic, binding, slot);
        written.emplace_back(binding.address, binding.expected.size());
    }
    bytes.resize(static_cast<std::size_t>(nativeOffset + nativeBytes), 0);
    headers.push_back({1, 6, nativeOffset, nativeBase, nativeBase, nativeBytes, align(nativeBytes, page), page});
    source = std::move(bytes);
    result.DynamicSection = std::move(dynamic);
    result.OriginalHeaders = std::move(headers);
}
}
