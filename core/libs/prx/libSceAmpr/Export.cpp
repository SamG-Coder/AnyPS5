#include <cstdint>
#include <cstddef>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libkernel/Apr/include/AprCommandBuffer.hpp"
#include <cstring>
#include <stdexcept>

static constexpr int SCE_AMPR_ERROR_BUFFER_FULL = 0x8002001C;
static constexpr int SCE_KERNEL_ERROR_EINVAL = 0x80020016;

static int Append(Apr::CommandBufferObject* buffer, const void* command, uint32_t bytes) {
    if (!buffer->base || bytes > buffer->size - buffer->offset) return SCE_AMPR_ERROR_BUFFER_FULL;
    std::memcpy(buffer->base + buffer->offset, command, bytes);
    buffer->offset += bytes;
    ++buffer->numCommands;
    return 0;
}

template<class TCommand>
static int AppendCommand(Apr::CommandBufferObject* buffer, Apr::Opcode opcode, TCommand command) {
    if (!buffer) return static_cast<int>(0x80020016);
    command.header = {opcode, sizeof(command)};
    return Append(buffer, &command, sizeof(command));
}

static bool ValidCounter(std::uint32_t counter) {
    return counter < 128u;
}

static bool ValidWriteAddress(volatile std::uint64_t* address) {
    return address && (reinterpret_cast<std::uintptr_t>(address) & 7u) == 0u;
}

static bool ValidWait(std::uint32_t compare, std::uint32_t flush) {
    return compare < 4u && flush < 2u;
}

static bool ValidWaitOnCounter_04_00(std::uint32_t access, std::uint32_t compare, std::uint32_t maskOperation, std::uint32_t flush) {
    return access < 8u && compare <= 6u && maskOperation < 2u && flush < 2u;
}

static bool ValidWriteCounter_04_00(std::uint32_t counter, std::uint32_t access, std::uint32_t operation) {
    return (counter & 0x80u) == 0u && access < 8u && operation < 5u;
}

static constexpr std::uint64_t MeasureInvalid = static_cast<std::uint32_t>(SCE_KERNEL_ERROR_EINVAL);

static std::uint64_t NopBytes(std::uint32_t dwords) {
    return sizeof(Apr::CommandHeader) + std::uint64_t{dwords} * 4u;
}

static int AppendNop(Apr::CommandBufferObject* buffer, std::uint32_t dwords, const std::uint32_t* data) {
    if (!buffer) return SCE_KERNEL_ERROR_EINVAL;
    const std::uint64_t bytes = NopBytes(dwords);
    if (!buffer->base || bytes > buffer->size - buffer->offset) return SCE_AMPR_ERROR_BUFFER_FULL;
    const Apr::CommandHeader header{Apr::Opcode::Nop, static_cast<std::uint32_t>(bytes)};
    std::uint8_t* destination = buffer->base + buffer->offset;
    std::memset(destination, 0, bytes);
    std::memcpy(destination, &header, sizeof(header));
    if (data) std::memcpy(destination + sizeof(header), data, std::uint64_t{dwords} * 4u);
    buffer->offset += static_cast<std::uint32_t>(bytes);
    ++buffer->numCommands;
    return 0;
}

static std::uint64_t MarkerBytes(const char* text) {
    return sizeof(Apr::MarkerCommand) + ((std::strlen(text) + 8) & ~std::uint64_t{7});
}

static std::uint64_t MeasureMarker(const char* text) {
    if (!text) return static_cast<std::uint32_t>(SCE_KERNEL_ERROR_EINVAL);
    return MarkerBytes(text);
}

static int AppendMarker(Apr::CommandBufferObject* buffer, Apr::Opcode opcode, const char* text) {
    if (!buffer || !text) return SCE_KERNEL_ERROR_EINVAL;
    const std::uint64_t bytes = MarkerBytes(text);
    if (!buffer->base || bytes > buffer->size - buffer->offset) return SCE_AMPR_ERROR_BUFFER_FULL;
    const Apr::MarkerCommand command{{opcode, static_cast<std::uint32_t>(bytes)}};
    std::uint8_t* destination = buffer->base + buffer->offset;
    std::memset(destination, 0, bytes);
    std::memcpy(destination, &command, sizeof(command));
    std::memcpy(destination + sizeof(command), text, std::strlen(text));
    buffer->offset += static_cast<std::uint32_t>(bytes);
    ++buffer->numCommands;
    return 0;
}

extern "C" {

int APS5_VABI sceAmprCommandBufferWriteAddressOnCompletion(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint64_t value) {
    if (!ValidWriteAddress(address)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteAddress, Apr::WriteAddressCommand{{}, reinterpret_cast<std::uint64_t>(address), value, 0, 0});
}

int APS5_VABI sceAmprCommandBufferWriteCounterOnCompletion(Apr::CommandBufferObject* buffer, std::uint8_t counter, std::uint32_t value) {
    if (!ValidCounter(counter)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteCounter, Apr::WriteCounterCommand{{}, counter, Apr::CounterAccess::Size4, Apr::CounterOperation::Store, 0, value});
}

int APS5_VABI sceAmprCommandBufferWaitOnAddress(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint64_t reference, std::uint8_t compare, std::uint8_t flush) {
    if ((reinterpret_cast<std::uintptr_t>(address) & 7u) != 0u || !ValidWait(compare, flush)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WaitOnAddress, Apr::WaitCommand{{}, reinterpret_cast<std::uint64_t>(address), reference, ~0ull, 0, compare});
}

int APS5_VABI sceAmprCommandBufferWaitOnCounter(Apr::CommandBufferObject* buffer, std::uint8_t counter, std::uint32_t reference, std::uint8_t compare, std::uint8_t flush) {
    if (!ValidCounter(counter) || !ValidWait(compare, flush)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WaitOnCounter, Apr::WaitCommand{{}, 0, reference, ~0ull, counter, compare, Apr::CounterAccess::Size4, 0});
}

int APS5_VABI sceAmprCommandBufferWriteKernelEventQueueOnCompletion(Apr::CommandBufferObject* buffer, std::uint64_t equeue, std::int32_t ident, std::uint64_t data) {
    if (!equeue) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteKernelEventQueue, Apr::WriteKernelEventQueueCommand{{}, equeue, static_cast<std::uint64_t>(ident), data, 0});
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromTimeCounterOnCompletion(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address) {
    if (!ValidWriteAddress(address)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteAddressFromTimeCounter, Apr::WriteAddressFromCounterCommand{{}, reinterpret_cast<std::uint64_t>(address), 0, 0});
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterOnCompletion(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint8_t counter) {
    if (!ValidWriteAddress(address) || !ValidCounter(counter)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteAddressFromCounter, Apr::WriteAddressFromCounterCommand{{}, reinterpret_cast<std::uint64_t>(address), counter, 0});
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint8_t counter) {
    if (!ValidWriteAddress(address) || (counter & 0x81u) != 0u) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteAddressFromCounterPair, Apr::WriteAddressFromCounterCommand{{}, reinterpret_cast<std::uint64_t>(address), counter, counter + 1u});
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressOnCompletion() { return sizeof(Apr::WriteAddressCommand); }
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteCounterOnCompletion() { return sizeof(Apr::WriteCounterCommand); }
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnAddress() { return sizeof(Apr::WaitCommand); }
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnCounter() { return sizeof(Apr::WaitCommand); }
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteKernelEventQueueOnCompletion() { return sizeof(Apr::WriteKernelEventQueueCommand); }

int APS5_VABI sceAmprAprCommandBufferConstructor(Apr::CommandBufferObject* buffer, uint64_t* gatherState, uint64_t* scatterState) {
    buffer->type = Apr::BufferType::Apr;
    *gatherState = 0;
    *scatterState = 0;
    return 0;
}

int APS5_VABI sceAmprAprCommandBufferDestructor(Apr::CommandBufferObject* buffer, uint64_t* gatherState, uint64_t* scatterState) {
    (void)buffer;
    (void)gatherState;
    (void)scatterState;
    return 0;
}

int APS5_VABI sceAmprAprCommandBufferMapBegin() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferMapDirectBegin() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferMapEnd() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferReadFile(Apr::CommandBufferObject* buffer, uint64_t* gatherState, uint64_t* scatterState, uint32_t fileId, void* destination, uint64_t size, uint64_t offset) {
    (void)gatherState;
    (void)scatterState;
    Apr::ReadFileCommand command{};
    command.header = {Apr::Opcode::ReadFile, sizeof(command)};
    command.fileId = fileId;
    command.destination = reinterpret_cast<uint64_t>(destination);
    command.size = size;
    command.offset = offset;
    return Append(buffer, &command, sizeof(command));
}

int APS5_VABI sceAmprAprCommandBufferReadFileGather() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferReadFileGatherScatter() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferReadFileScatter() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprAprCommandBufferResetGatherScatterState() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprCommandBufferClearBuffer(Apr::CommandBufferObject* buffer) {
    buffer->base = nullptr;
    buffer->size = 0;
    buffer->offset = 0;
    buffer->numCommands = 0;
    return 0;
}

int APS5_VABI sceAmprCommandBufferConstructMarker(Apr::CommandBufferObject* buffer, std::uint32_t type, const char* text, const std::uint32_t* color) {
    switch (type) {
        case 1: return AppendMarker(buffer, Apr::Opcode::SetMarker, text);
        case 2: return AppendMarker(buffer, Apr::Opcode::PushMarker, text);
        case 3: return AppendCommand(buffer, Apr::Opcode::PopMarker, Apr::MarkerCommand{});
        case 5: return color ? AppendMarker(buffer, Apr::Opcode::SetMarker, text) : SCE_KERNEL_ERROR_EINVAL;
        case 6: return color ? AppendMarker(buffer, Apr::Opcode::PushMarker, text) : SCE_KERNEL_ERROR_EINVAL;
        default: return SCE_KERNEL_ERROR_EINVAL;
    }
}

int APS5_VABI sceAmprCommandBufferConstructNop(Apr::CommandBufferObject* buffer, std::int16_t type, const void* payload, std::uint32_t payloadBytes, const std::uint32_t* word) {
    (void)type;
    if (payloadBytes > (word ? 56u : 60u)) return SCE_KERNEL_ERROR_EINVAL;
    if (payloadBytes != 0u && !payload) throw std::invalid_argument("sceAmprCommandBufferConstructNop: null payload");
    const std::uint32_t dwords = (payloadBytes + 3u) / 4u + (word ? 1u : 0u);
    const std::uint32_t offset = buffer ? buffer->offset : 0u;
    const int result = AppendNop(buffer, dwords, nullptr);
    if (result != 0) return result;
    std::uint8_t* data = buffer->base + offset + sizeof(Apr::CommandHeader);
    if (word) {
        std::memcpy(data, word, sizeof(*word));
        data += sizeof(*word);
    }
    if (payloadBytes != 0u) std::memcpy(data, payload, payloadBytes);
    return 0;
}

int APS5_VABI sceAmprCommandBufferConstructor(Apr::CommandBufferObject* buffer) {
    *buffer = {nullptr, 0, 0, 0, Apr::BufferType::Generic};
    return 0;
}

int APS5_VABI sceAmprCommandBufferDestructor(Apr::CommandBufferObject* buffer) {
    (void)buffer;
    return 0;
}

void* APS5_VABI sceAmprCommandBufferGetBufferBaseAddress(const Apr::CommandBufferObject* buffer) {
    return buffer->base;
}

uint32_t APS5_VABI sceAmprCommandBufferGetCurrentOffset(const Apr::CommandBufferObject* buffer) {
    return buffer->offset;
}

uint32_t APS5_VABI sceAmprCommandBufferGetNumCommands(const Apr::CommandBufferObject* buffer) {
    return buffer->numCommands;
}

uint32_t APS5_VABI sceAmprCommandBufferGetSize(const Apr::CommandBufferObject* buffer) {
    return buffer->size;
}

uint32_t APS5_VABI sceAmprCommandBufferGetType(const Apr::CommandBufferObject* buffer) {
    return static_cast<uint32_t>(buffer->type);
}

int APS5_VABI sceAmprCommandBufferNop(Apr::CommandBufferObject* buffer, std::uint32_t dwords) {
    if (dwords == 0u || dwords > 16u) return SCE_KERNEL_ERROR_EINVAL;
    return AppendNop(buffer, dwords, nullptr);
}

int APS5_VABI sceAmprCommandBufferNopWithData(Apr::CommandBufferObject* buffer, std::uint32_t dwords, const std::uint32_t* data) {
    if (dwords > 15u) return SCE_KERNEL_ERROR_EINVAL;
    if (dwords != 0u && !data) throw std::invalid_argument("sceAmprCommandBufferNopWithData: null data");
    return AppendNop(buffer, dwords, data);
}

int APS5_VABI sceAmprCommandBufferPopMarker(Apr::CommandBufferObject* buffer) {
    return AppendCommand(buffer, Apr::Opcode::PopMarker, Apr::MarkerCommand{});
}

int APS5_VABI sceAmprCommandBufferPushMarker(Apr::CommandBufferObject* buffer, const char* text) {
    return AppendMarker(buffer, Apr::Opcode::PushMarker, text);
}

int APS5_VABI sceAmprCommandBufferPushMarkerWithColor(Apr::CommandBufferObject* buffer, const char* text, std::uint32_t color) {
    (void)color;
    return AppendMarker(buffer, Apr::Opcode::PushMarker, text);
}

int APS5_VABI sceAmprCommandBufferReset(Apr::CommandBufferObject* buffer) {
    buffer->offset = 0;
    buffer->numCommands = 0;
    return 0;
}

int APS5_VABI sceAmprCommandBufferSetBuffer(Apr::CommandBufferObject* buffer, void* memory, uint32_t size) {
    buffer->base = static_cast<uint8_t*>(memory);
    buffer->size = size;
    buffer->offset = 0;
    buffer->numCommands = 0;
    return 0;
}

int APS5_VABI sceAmprCommandBufferSetMarker(Apr::CommandBufferObject* buffer, const char* text) {
    return AppendMarker(buffer, Apr::Opcode::SetMarker, text);
}

int APS5_VABI sceAmprCommandBufferSetMarkerWithColor(Apr::CommandBufferObject* buffer, const char* text, const std::uint32_t* color) {
    if (!color) return SCE_KERNEL_ERROR_EINVAL;
    return AppendMarker(buffer, Apr::Opcode::SetMarker, text);
}

int APS5_VABI sceAmprCommandBufferWaitOnAddress_04_00(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint64_t reference, std::uint8_t compare, std::uint8_t flush) {
    if (!ValidWriteAddress(address) || compare > 6u || flush > 1u) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WaitOnAddress, Apr::WaitCommand{{}, reinterpret_cast<std::uint64_t>(address), reference, ~0ull, 0, compare});
}

int APS5_VABI sceAmprCommandBufferWaitOnCounter_04_00(Apr::CommandBufferObject* buffer, std::uint8_t counter, std::uint8_t access, std::uint64_t reference, std::uint8_t compare, std::uint8_t maskOperation, std::uint64_t mask, std::uint8_t flush) {
    if (!ValidWaitOnCounter_04_00(access, compare, maskOperation, flush)) return SCE_KERNEL_ERROR_EINVAL;
    const std::uint64_t applied = maskOperation ? mask : ~0ull;
    return AppendCommand(buffer, Apr::Opcode::WaitOnCounter, Apr::WaitCommand{{}, 0, reference, applied, counter, compare, static_cast<Apr::CounterAccess>(access), 0});
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterPair_04_00(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint8_t counter, std::uint64_t atStart) {
    (void)atStart;
    return sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(buffer, address, counter);
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromCounter_04_00(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint8_t counter, std::uint64_t atStart) {
    (void)atStart;
    return sceAmprCommandBufferWriteAddressFromCounterOnCompletion(buffer, address, counter);
}

int APS5_VABI sceAmprCommandBufferWriteAddressFromTimeCounter_04_00(Apr::CommandBufferObject* buffer, volatile std::uint64_t* address, std::uint64_t atStart) {
    (void)atStart;
    return sceAmprCommandBufferWriteAddressFromTimeCounterOnCompletion(buffer, address);
}

int APS5_VABI sceAmprCommandBufferWriteAddress_04_00(Apr::CommandBufferObject* buffer, uint64_t* address, uint64_t value, uint32_t flags) {
    Apr::WriteAddressCommand command{};
    command.header = {Apr::Opcode::WriteAddress, sizeof(command)};
    command.address = reinterpret_cast<uint64_t>(address);
    command.value = value;
    command.flags = flags;
    return Append(buffer, &command, sizeof(command));
}

int APS5_VABI sceAmprCommandBufferWriteCounter_04_00(Apr::CommandBufferObject* buffer, std::uint8_t counter, std::uint8_t access, std::uint64_t value, std::uint8_t operation, std::uint8_t atStart) {
    (void)atStart;
    if (!ValidWriteCounter_04_00(counter, access, operation)) return SCE_KERNEL_ERROR_EINVAL;
    return AppendCommand(buffer, Apr::Opcode::WriteCounter, Apr::WriteCounterCommand{{}, counter, static_cast<Apr::CounterAccess>(access), static_cast<Apr::CounterOperation>(operation), 0, value});
}

int APS5_VABI sceAmprCommandBufferWriteKernelEventQueue_04_00(Apr::CommandBufferObject* buffer, std::uint64_t equeue, std::int32_t ident, std::uint64_t data, std::uint64_t atStart) {
    (void)atStart;
    return sceAmprCommandBufferWriteKernelEventQueueOnCompletion(buffer, equeue, ident, data);
}

int APS5_VABI sceAmprMeasureCommandSizeMapBegin() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprMeasureCommandSizeMapDirectBegin() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprMeasureCommandSizeMapEnd() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeNop(std::uint32_t dwords) {
    if (dwords == 0u || dwords > 16u) return MeasureInvalid;
    return NopBytes(dwords);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeNopWithData(std::uint32_t dwords) {
    if (dwords == 0u || dwords > 16u) return MeasureInvalid;
    return NopBytes(dwords - 1u);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizePopMarker() {
    return sizeof(Apr::MarkerCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizePushMarker(const char* text) {
    return MeasureMarker(text);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizePushMarkerWithColor(const char* text, std::uint32_t color) {
    (void)color;
    return MeasureMarker(text);
}

uint32_t APS5_VABI sceAmprMeasureCommandSizeReadFile(void) {
    return sizeof(Apr::ReadFileCommand);
}

int APS5_VABI sceAmprMeasureCommandSizeReadFileGather() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprMeasureCommandSizeReadFileGatherScatter() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprMeasureCommandSizeReadFileScatter() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

int APS5_VABI sceAmprMeasureCommandSizeResetGatherScatterState() {
 NotImplemented_nid_no_patch(__func__);
 return 0;
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeSetMarker(const char* text) {
    return MeasureMarker(text);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeSetMarkerWithColor(const char* text, std::uint32_t color) {
    (void)color;
    return MeasureMarker(text);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnAddress_04_00(volatile std::uint64_t* address, std::uint64_t, std::uint8_t compare, std::uint8_t flush) {
    if (!ValidWriteAddress(address) || compare > 6u || flush > 1u) return MeasureInvalid;
    return sizeof(Apr::WaitCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnCounter_04_00(std::uint8_t, std::uint8_t access, std::uint64_t, std::uint8_t compare, std::uint8_t maskOperation, std::uint64_t, std::uint8_t flush) {
    if (!ValidWaitOnCounter_04_00(access, compare, maskOperation, flush)) return MeasureInvalid;
    return sizeof(Apr::WaitCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromCounterPair_04_00(volatile std::uint64_t* address, std::uint8_t counter) {
    if (!ValidWriteAddress(address) || (counter & 1u) != 0u) return MeasureInvalid;
    return sizeof(Apr::WriteAddressFromCounterCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromCounter_04_00(volatile std::uint64_t* address, std::uint8_t counter) {
    (void)counter;
    if (!ValidWriteAddress(address)) return MeasureInvalid;
    return sizeof(Apr::WriteAddressFromCounterCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromTimeCounter_04_00(volatile std::uint64_t* address) {
    if (!ValidWriteAddress(address)) return MeasureInvalid;
    return sizeof(Apr::WriteAddressFromCounterCommand);
}

uint32_t APS5_VABI sceAmprMeasureCommandSizeWriteAddress_04_00(void) {
    return sizeof(Apr::WriteAddressCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteCounter_04_00(std::uint8_t counter, std::uint8_t access, std::uint64_t, std::uint8_t operation) {
    if (!ValidWriteCounter_04_00(counter, access, operation)) return MeasureInvalid;
    return sizeof(Apr::WriteCounterCommand);
}

std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteKernelEventQueue_04_00(std::uint64_t, std::int32_t, std::uint64_t) {
    return sizeof(Apr::WriteKernelEventQueueCommand);
}

}
