#ifndef CORE_LIBS_PRX_LIBKERNEL_APR_APRCOMMANDBUFFER_HPP
#define CORE_LIBS_PRX_LIBKERNEL_APR_APRCOMMANDBUFFER_HPP

#include <cstddef>
#include <cstdint>

// Shared between libSceAmpr (which records commands) and libkernel (which executes them).
// The guest treats both the command buffer object and its memory as opaque, so the encoding is ours.
namespace Apr {

enum class BufferType : std::uint32_t {
    Generic = 0,
    Apr = 1,
};

struct CommandBufferObject {
    std::uint8_t* base;
    std::uint32_t size;
    std::uint32_t offset;
    std::uint32_t numCommands;
    BufferType type;
};
static_assert(sizeof(CommandBufferObject) == 0x18, "guest reserves 0x18 bytes for sce::Ampr::CommandBuffer");

enum class Opcode : std::uint32_t {
    Nop = 0,
    ReadFile = 1,
    WriteAddress = 2,
    WriteCounter = 3,
    WaitOnAddress = 4,
    WaitOnCounter = 5,
    WriteKernelEventQueue = 6,
    WriteAddressFromTimeCounter = 7,
    WriteAddressFromCounter = 8,
    WriteAddressFromCounterPair = 9,
    PushMarker = 10,
    PopMarker = 11,
    SetMarker = 12,
};

struct CommandHeader {
    Opcode opcode;
    std::uint32_t bytes;
};

struct ReadFileCommand {
    CommandHeader header;
    std::uint32_t fileId;
    std::uint32_t reserved;
    std::uint64_t destination;
    std::uint64_t size;
    std::uint64_t offset;
};

struct WriteAddressCommand {
    CommandHeader header;
    std::uint64_t address;
    std::uint64_t value;
    std::uint32_t flags;
    std::uint32_t reserved;
};

enum class CounterAccess : std::uint8_t {
    Size8 = 0,
    Size4 = 1,
    Size2Offset0 = 2,
    Size2Offset1 = 3,
    Size1Offset0 = 4,
    Size1Offset1 = 5,
    Size1Offset2 = 6,
    Size1Offset3 = 7,
};

enum class CounterOperation : std::uint8_t {
    Store = 0,
    AtomicOr = 1,
    AtomicAndComplement = 2,
    AtomicXor = 3,
    AtomicAdd = 4,
};

struct WriteCounterCommand {
    CommandHeader header;
    std::uint32_t counter;
    CounterAccess access;
    CounterOperation operation;
    std::uint16_t reserved;
    std::uint64_t value;
};

struct WaitCommand {
    CommandHeader header;
    std::uint64_t address;
    std::uint64_t reference;
    std::uint64_t mask;
    std::uint32_t counter;
    std::uint8_t compare;
    CounterAccess access;
    std::uint16_t reserved;
};

struct WriteKernelEventQueueCommand {
    CommandHeader header;
    std::uint64_t equeue;
    std::uint64_t ident;
    std::uint64_t data;
    std::uint64_t userData;
};

struct WriteAddressFromCounterCommand {
    CommandHeader header;
    std::uint64_t address;
    std::uint32_t counter0;
    std::uint32_t counter1;
};

struct MarkerCommand {
    CommandHeader header;
};

}

#endif
