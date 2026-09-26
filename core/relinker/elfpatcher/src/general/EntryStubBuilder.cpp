#include <elfpatcher/general/EntryStubBuilder.hpp>
#include <elfpatcher/general/ElfConstants.hpp>
#include <limits>
#include <domain/Types.hpp>

namespace Elfpatcher {

namespace {
void _appendBytes(std::vector<std::uint8_t>& s, const std::uint8_t* bytes, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i)
        s.push_back(bytes[i]);
}
}

std::vector<std::uint8_t> EntryStubBuilder::BuildEntryStub(
    const std::uint64_t stubVaddr,
    const std::uint64_t realEntryVaddr
) const {
    std::vector<std::uint8_t> s;
    s.push_back(kStubOpPopRax);
    _appendBytes(s, kStubOpMovRbxRsp, sizeof(kStubOpMovRbxRsp));
    _appendBytes(s, kStubOpSubRsp0x30, sizeof(kStubOpSubRsp0x30));
    _appendBytes(s, kStubOpAndRsp0xf0, sizeof(kStubOpAndRsp0xf0));
    _appendBytes(s, kStubOpMovDwordPtrRsp, sizeof(kStubOpMovDwordPtrRsp));
    _appendBytes(s, kStubOpMovQwordPtrRsp8Rbx, sizeof(kStubOpMovQwordPtrRsp8Rbx));
    _appendBytes(s, kStubOpMovRdiRsp, sizeof(kStubOpMovRdiRsp));
    _appendBytes(s, kStubOpXorRsiRsi, sizeof(kStubOpXorRsiRsi));
    const std::uint64_t callInsnVaddr = stubVaddr + s.size();
    const std::uint64_t callNextVaddr = callInsnVaddr + kStubCallInstructionSize;
    const bool forward = realEntryVaddr >= callNextVaddr;
    const auto distance = forward ? realEntryVaddr - callNextVaddr : callNextVaddr - realEntryVaddr;
    if (distance > (forward ? 0x7fffffffull : 0x80000000ull))
        throw Domain::RelinkerException("Native entry point exceeds x86 rel32 reach");
    const auto rel32 = static_cast<std::int32_t>(forward ? static_cast<std::int64_t>(distance) : -static_cast<std::int64_t>(distance));
    s.push_back(kStubOpCallRel32);
    s.push_back(static_cast<std::uint8_t>(rel32 & 0xff));
    s.push_back(static_cast<std::uint8_t>((rel32 >> 8) & 0xff));
    s.push_back(static_cast<std::uint8_t>((rel32 >> 16) & 0xff));
    s.push_back(static_cast<std::uint8_t>((rel32 >> 24) & 0xff));
    // A returned native entry result is a process exit status, not an illegal
    // instruction. This syscall belongs to the generated Linux startup stub.
    constexpr std::uint8_t exitReturnedEntry[]{0x89, 0xc7, 0xb8, 0x3c, 0, 0, 0, 0x0f, 0x05};
    _appendBytes(s, exitReturnedEntry, sizeof(exitReturnedEntry));
    _appendBytes(s, kStubOpUd2, sizeof(kStubOpUd2));
    return s;
}

}
