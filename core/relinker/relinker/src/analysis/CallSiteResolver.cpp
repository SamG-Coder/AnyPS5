#include <relinker/analysis/CallSiteResolver.hpp>
#include <codegen/x86/X64InstructionDecoder.hpp>
#include <algorithm>
#include <cstring>
#include <limits>
#include <map>
#include <optional>
#include <set>

namespace Relinker {
namespace {

std::optional<VirtualAddress> relativeTarget(VirtualAddress end, std::int32_t displacement) {
    if (displacement >= 0) {
        if (static_cast<std::uint64_t>(displacement) > UINT64_MAX - end) return std::nullopt;
        return end + displacement;
    }
    const auto distance = static_cast<std::uint64_t>(-static_cast<std::int64_t>(displacement));
    if (distance > end) return std::nullopt;
    return end - distance;
}

struct Reference {
    VirtualAddress instruction;
    VirtualAddress target;
    bool indirect;
    bool jump;
};

class CallSiteResolver final : public ICallSiteResolver {
public:
    std::vector<FileByteOffset> ResolveCallSites(const std::vector<std::uint8_t>& text,
        FileByteOffset base, VirtualAddress target, ByteCount size) override {
        if (size > UINT64_MAX - target || text.size() > UINT64_MAX - base)
            throw RelinkerException("Call-site address range overflow");
        // Cache the bytes as well as their load address: callers may reuse the
        // same vector object for a different image. Never cache by pointer alone.
        if (!initialized || cachedBase != base || cachedText != text) rebuild(text, base);
        std::set<FileByteOffset> found;
        const auto first = targetToSites.lower_bound(target);
        const auto last = targetToSites.lower_bound(target + size);
        for (auto it = first; it != last; ++it) found.insert(it->second.begin(), it->second.end());
        return {found.begin(), found.end()};
    }

private:
    bool initialized = false;
    VirtualAddress cachedBase = 0;
    std::vector<std::uint8_t> cachedText;
    std::map<VirtualAddress, std::vector<FileByteOffset>> targetToSites;

    void rebuild(const std::vector<std::uint8_t>& text, VirtualAddress base) {
        std::vector<Reference> references;
        Codegen::X64InstructionDecoder decoder;
        for (std::size_t offset = 0; offset < text.size();) {
            const auto length = decoder.Decode(text.data() + offset, text.size() - offset);
            if (length == 0 || length > text.size() - offset) throw RelinkerException("Invalid instruction length", base + offset);
            // Decode only canonical x86-64 calls/tail calls. MOV/LEA of an import
            // address is an escape, not a proven call site.
            std::size_t prefix = 0;
            if (text[offset] == 0xf2 || text[offset] == 0x3e) ++prefix; // BND / notrack
            if (prefix < length && text[offset + prefix] >= 0x40 && text[offset + prefix] <= 0x4f) ++prefix;
            const auto op = prefix < length ? text[offset + prefix] : 0;
            std::size_t field = 0;
            bool indirect = false, jump = false;
            if ((op == 0xe8 || op == 0xe9) && length == prefix + 5) {
                field = offset + prefix + 1;
                jump = op == 0xe9;
            } else if (op == 0xff && length == prefix + 6 &&
                       (text[offset + prefix + 1] == 0x15 || text[offset + prefix + 1] == 0x25)) {
                field = offset + prefix + 2;
                indirect = true;
                jump = text[offset + prefix + 1] == 0x25;
            }
            if (field != 0) {
                std::int32_t displacement;
                std::memcpy(&displacement, text.data() + field, sizeof(displacement));
                if (const auto destination = relativeTarget(base + offset + length, displacement))
                    references.push_back({base + offset, *destination, indirect, jump});
            }
            offset += length;
        }
        std::map<VirtualAddress, VirtualAddress> thunks;
        for (const auto& reference : references)
            if (reference.indirect && reference.jump) thunks.emplace(reference.instruction, reference.target);
        std::map<VirtualAddress, std::vector<FileByteOffset>> index;
        for (const auto& reference : references) {
            if (reference.indirect) index[reference.target].push_back(reference.instruction);
            else if (const auto thunk = thunks.find(reference.target); thunk != thunks.end())
                index[thunk->second].push_back(reference.instruction);
        }
        cachedText = text;
        cachedBase = base;
        targetToSites = std::move(index);
        initialized = true;
    }
};
}
std::shared_ptr<ICallSiteResolver> MakeCallSiteResolver() { return std::make_shared<CallSiteResolver>(); }
}
