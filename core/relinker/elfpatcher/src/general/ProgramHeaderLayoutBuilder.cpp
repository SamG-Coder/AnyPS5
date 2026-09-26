#include <elfpatcher/general/ProgramHeaderLayoutBuilder.hpp>
#include <elfpatcher/general/ElfConstants.hpp>
#include <domain/Types.hpp>
#include <cstdint>
#include <string>
#include <limits>
#include <algorithm>

namespace Elfpatcher {

ProgramHeaderLayoutBuilder::ProgramHeaderLayoutBuilder(
    std::shared_ptr<ISegmentFilter> segmentFilter,
    std::shared_ptr<Io::IByteWriter> byteWriter
)
    : _segmentFilter(std::move(segmentFilter))
    , _byteWriter(std::move(byteWriter))
{
}

void ProgramHeaderLayoutBuilder::_writeProgramHeader(std::vector<std::uint8_t>& buf, std::size_t offset, const Domain::ProgramHeader& ph) const {
    _byteWriter->WriteU32(buf, offset + kPhdrTypeOffset, ph.Type);
    _byteWriter->WriteU32(buf, offset + kPhdrFlagsOffset, ph.Flags);
    _byteWriter->WriteU64(buf, offset + kPhdrOffsetOffset, ph.Offset);
    _byteWriter->WriteU64(buf, offset + kPhdrVaddrOffset, ph.MappedAddress);
    _byteWriter->WriteU64(buf, offset + kPhdrPaddrOffset, ph.PhysicalAddress);
    _byteWriter->WriteU64(buf, offset + kPhdrFileSizeOffset, ph.FileSize);
    _byteWriter->WriteU64(buf, offset + kPhdrMemSizeOffset, ph.MemorySize);
    _byteWriter->WriteU64(buf, offset + kPhdrAlignOffset, ph.Alignment);
}

Domain::ProgramHeader ProgramHeaderLayoutBuilder::_makeLoadHeader(std::uint64_t offset, std::uint64_t vaddr, std::uint64_t size) const {
    Domain::ProgramHeader ph{};
    ph.Type = PT_LOAD;
    ph.Flags = PF_R | PF_W | PF_X;
    ph.Offset = offset;
    ph.MappedAddress = vaddr;
    ph.PhysicalAddress = vaddr;
    ph.FileSize = size;
    ph.MemorySize = size;
    ph.Alignment = kDefaultLoadAlignment;
    return ph;
}

Domain::ProgramHeader ProgramHeaderLayoutBuilder::_makeHeaderBlockLoad(std::uint64_t vaddr, std::uint64_t size, std::uint64_t align) const {
    Domain::ProgramHeader ph{};
    ph.Type = PT_LOAD;
    ph.Flags = PF_R;
    ph.Offset = 0;
    ph.MappedAddress = vaddr;
    ph.PhysicalAddress = vaddr;
    ph.FileSize = size;
    ph.MemorySize = size;
    ph.Alignment = align;
    return ph;
}

Domain::ProgramHeader ProgramHeaderLayoutBuilder::_makePhdrHeader(std::uint64_t offset, std::uint64_t vaddr, std::uint64_t size) const {
    Domain::ProgramHeader ph{};
    ph.Type = PT_PHDR;
    ph.Flags = PF_R;
    ph.Offset = offset;
    ph.MappedAddress = vaddr;
    ph.PhysicalAddress = vaddr;
    ph.FileSize = size;
    ph.MemorySize = size;
    ph.Alignment = kPhdrHeaderAlignment;
    return ph;
}

Domain::ProgramHeader ProgramHeaderLayoutBuilder::_makeDynamicHeader(std::uint64_t offset, std::uint64_t vaddr, std::uint64_t size) const {
    Domain::ProgramHeader ph{};
    ph.Type = PT_DYNAMIC;
    ph.Flags = PF_R | PF_W;
    ph.Offset = offset;
    ph.MappedAddress = vaddr;
    ph.PhysicalAddress = vaddr;
    ph.FileSize = size;
    ph.MemorySize = size;
    ph.Alignment = kDynamicHeaderAlignment;
    return ph;
}

Domain::ProgramHeader ProgramHeaderLayoutBuilder::_makeInterpHeader(std::uint64_t offset, std::uint64_t vaddr, std::uint64_t size) const {
    Domain::ProgramHeader ph{};
    ph.Type = PT_INTERP;
    ph.Flags = PF_R;
    ph.Offset = offset;
    ph.MappedAddress = vaddr;
    ph.PhysicalAddress = vaddr;
    ph.FileSize = size;
    ph.MemorySize = size;
    ph.Alignment = kInterpHeaderAlignment;
    return ph;
}

std::uint32_t ProgramHeaderLayoutBuilder::_fixLoadFlags(std::uint32_t originalFlags) const {
    if (originalFlags == 0) return PF_R | PF_W | PF_X;
    return originalFlags | PF_R;
}

std::uint64_t ProgramHeaderLayoutBuilder::ComputeExtraBlockVaddr(
    const std::vector<Domain::ProgramHeader>& originalHeaders,
    std::uint64_t extraBlockOffset
) const {
    bool foundLoad = false;
    std::uint64_t highestVaddrEnd = 0;
    for (const auto& ph : originalHeaders) {
        if (_segmentFilter->ShouldSkip(ph))
            continue;
        if (ph.Type != PT_LOAD)
            continue;
        foundLoad = true;
        const std::uint64_t vaddrEnd = ph.MappedAddress + ph.MemorySize;
        if (vaddrEnd > highestVaddrEnd)
            highestVaddrEnd = vaddrEnd;
    }
    if (!foundLoad)
        throw Domain::RelinkerException("No kept PT_LOAD segment to anchor the extra block against");

    const std::uint64_t align = kDefaultLoadAlignment;
    const std::uint64_t offsetRemainder = extraBlockOffset & (align - 1);
    const std::uint64_t alignedFloor = highestVaddrEnd & ~(align - 1);
    std::uint64_t vaddr = alignedFloor | offsetRemainder;
    if (vaddr < highestVaddrEnd)
        vaddr += align;
    if (vaddr < highestVaddrEnd)
        throw Domain::RelinkerException("Extra block vaddr computation overflowed");
    return vaddr;
}

std::uint16_t ProgramHeaderLayoutBuilder::WriteLayout(
    std::vector<std::uint8_t>& buf,
    const ProgramHeaderLayoutRequest& request
) const {
    std::uint32_t keptCount = 0;
    for (const auto& ph : request.OriginalHeaders) {
        if (_segmentFilter->ShouldSkip(ph) || ph.Type == PT_PHDR || ph.Type == PT_INTERP)
            continue;
        keptCount++;
    }

    const auto count = keptCount + kSyntheticProgramHeaderCount;
    if (count > std::numeric_limits<std::uint16_t>::max() || request.PhEntSize != 56)
        throw Domain::RelinkerException("Unsupported native program header count or size");
    const auto neededPh = static_cast<std::uint16_t>(count);
    if (request.ExtraBlockOffset > buf.size() || request.ExtraBlockSize != buf.size() - request.ExtraBlockOffset)
        throw Domain::RelinkerException("Extra block must end at the current native image boundary");
    // New native imports/segments need not fit the source ELF's header capacity.
    // Append the output table instead of overwriting source program bytes.
    if (buf.size() > std::numeric_limits<std::size_t>::max() - 7u - static_cast<std::size_t>(neededPh) * 56u)
        throw Domain::RelinkerException("Native program header table size overflow");
    const auto nativePhOff = (buf.size() + 7u) & ~std::size_t{7u};
    buf.resize(nativePhOff + static_cast<std::size_t>(neededPh) * 56u, 0);
    auto nativeRequest = request;
    nativeRequest.PhOff = nativePhOff;
    nativeRequest.PhNum = neededPh;
    nativeRequest.ExtraBlockSize = buf.size() - request.ExtraBlockOffset;
    _byteWriter->WriteU64(buf, kEhdrPhOffOffset, nativePhOff);

    if (nativeRequest.DynamicSegmentOffset < nativeRequest.ExtraBlockOffset)
        throw Domain::RelinkerException("Dynamic segment offset lies before the extra block");
    if (nativeRequest.DynamicSegmentOffset + nativeRequest.DynamicSegmentSize > nativeRequest.ExtraBlockOffset + nativeRequest.ExtraBlockSize)
        throw Domain::RelinkerException("Dynamic segment does not fit within the extra block");
    if (nativeRequest.InterpOffset < nativeRequest.ExtraBlockOffset)
        throw Domain::RelinkerException("Interp offset lies before the extra block");
    if (nativeRequest.InterpOffset + nativeRequest.InterpSize > nativeRequest.ExtraBlockOffset + nativeRequest.ExtraBlockSize)
        throw Domain::RelinkerException("Interp data does not fit within the extra block");

    const std::uint64_t dynamicSegmentVaddr = nativeRequest.ExtraBlockVaddr + (nativeRequest.DynamicSegmentOffset - nativeRequest.ExtraBlockOffset);
    const std::uint64_t interpVaddr = nativeRequest.ExtraBlockVaddr + (nativeRequest.InterpOffset - nativeRequest.ExtraBlockOffset);

    // The output table is mapped by the extra PT_LOAD itself. An additional
    // high-address alias of file offset zero is unnecessary and confuses load
    // bias calculation when it precedes the application's lower-address loads.
    const auto phdrVaddr = nativeRequest.ExtraBlockVaddr +
        (nativeRequest.PhOff - nativeRequest.ExtraBlockOffset);
    std::vector<Domain::ProgramHeader> headers;
    headers.reserve(neededPh);
    headers.push_back(_makePhdrHeader(nativeRequest.PhOff, phdrVaddr,
        static_cast<std::uint64_t>(neededPh) * nativeRequest.PhEntSize));
    headers.push_back(_makeInterpHeader(nativeRequest.InterpOffset, interpVaddr, nativeRequest.InterpSize));
    std::vector<Domain::ProgramHeader> loads;
    std::vector<Domain::ProgramHeader> other;
    for (const auto& ph : nativeRequest.OriginalHeaders) {
        if (_segmentFilter->ShouldSkip(ph) || ph.Type == PT_PHDR || ph.Type == PT_INTERP)
            continue;
        if (ph.Type == PT_LOAD) {
            auto fixed = ph;
            fixed.Flags = _fixLoadFlags(ph.Flags);
            loads.push_back(fixed);
        } else {
            other.push_back(ph);
        }
    }
    loads.push_back(_makeLoadHeader(nativeRequest.ExtraBlockOffset,
        nativeRequest.ExtraBlockVaddr, nativeRequest.ExtraBlockSize));
    std::stable_sort(loads.begin(), loads.end(), [](const auto& a, const auto& b) {
        return a.MappedAddress < b.MappedAddress;
    });
    headers.insert(headers.end(), loads.begin(), loads.end());
    headers.insert(headers.end(), other.begin(), other.end());
    headers.push_back(_makeDynamicHeader(nativeRequest.DynamicSegmentOffset,
        dynamicSegmentVaddr, nativeRequest.DynamicSegmentSize));
    if (headers.size() != neededPh)
        throw Domain::RelinkerException("Native program header count disagrees with allocated table");
    for (std::size_t i = 0; i < headers.size(); ++i)
        _writeProgramHeader(buf, static_cast<std::size_t>(nativeRequest.PhOff) +
            i * nativeRequest.PhEntSize, headers[i]);
    return neededPh;
}

}
