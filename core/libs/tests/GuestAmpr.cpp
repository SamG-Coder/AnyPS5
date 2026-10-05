#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libkernel/Apr/include/AprCommandBuffer.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <future>
#include <string>
#include <utility>

extern "C" {
int APS5_VABI sceAmprCommandBufferConstructor(Apr::CommandBufferObject*);
int APS5_VABI sceAmprAprCommandBufferConstructor(Apr::CommandBufferObject*, std::uint64_t*, std::uint64_t*);
int APS5_VABI sceAmprCommandBufferSetBuffer(Apr::CommandBufferObject*, void*, std::uint32_t);
std::uint32_t APS5_VABI sceAmprCommandBufferGetCurrentOffset(const Apr::CommandBufferObject*);
std::uint32_t APS5_VABI sceAmprCommandBufferGetNumCommands(const Apr::CommandBufferObject*);
int APS5_VABI sceAmprCommandBufferWriteAddressOnCompletion(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint64_t);
int APS5_VABI sceAmprCommandBufferPushMarker(Apr::CommandBufferObject*, const char*);
int APS5_VABI sceAmprCommandBufferPushMarkerWithColor(Apr::CommandBufferObject*, const char*, std::uint32_t);
int APS5_VABI sceAmprCommandBufferPopMarker(Apr::CommandBufferObject*);
int APS5_VABI sceAmprCommandBufferSetMarker(Apr::CommandBufferObject*, const char*);
int APS5_VABI sceAmprCommandBufferSetMarkerWithColor(Apr::CommandBufferObject*, const char*, const std::uint32_t*);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizePushMarker(const char*);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizePushMarkerWithColor(const char*, std::uint32_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizePopMarker();
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeSetMarker(const char*);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeSetMarkerWithColor(const char*, std::uint32_t);
int APS5_VABI sceKernelAprSubmitCommandBuffer(const Apr::CommandBufferObject*, std::uint32_t);
int APS5_VABI sceAmprCommandBufferWaitOnAddress(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint64_t, std::uint8_t, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWaitOnCounter(Apr::CommandBufferObject*, std::uint8_t, std::uint32_t, std::uint8_t, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWriteCounterOnCompletion(Apr::CommandBufferObject*, std::uint8_t, std::uint32_t);
int APS5_VABI sceAmprCommandBufferWriteAddressFromTimeCounterOnCompletion(Apr::CommandBufferObject*, volatile std::uint64_t*);
int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterOnCompletion(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWriteKernelEventQueueOnCompletion(Apr::CommandBufferObject*, std::uint64_t, std::int32_t, std::uint64_t);
int APS5_VABI sceAmprCommandBufferNop(Apr::CommandBufferObject*, std::uint32_t);
int APS5_VABI sceAmprCommandBufferNopWithData(Apr::CommandBufferObject*, std::uint32_t, const std::uint32_t*);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeNop(std::uint32_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeNopWithData(std::uint32_t);
int APS5_VABI sceAmprCommandBufferWaitOnAddress_04_00(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint64_t, std::uint8_t, std::uint8_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnAddress_04_00(volatile std::uint64_t*, std::uint64_t, std::uint8_t, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWriteAddressFromTimeCounter_04_00(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint64_t);
int APS5_VABI sceAmprCommandBufferWriteAddressFromCounter_04_00(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint8_t, std::uint64_t);
int APS5_VABI sceAmprCommandBufferWriteAddressFromCounterPair_04_00(Apr::CommandBufferObject*, volatile std::uint64_t*, std::uint8_t, std::uint64_t);
int APS5_VABI sceAmprCommandBufferWriteKernelEventQueue_04_00(Apr::CommandBufferObject*, std::uint64_t, std::int32_t, std::uint64_t, std::uint64_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromTimeCounter_04_00(volatile std::uint64_t*);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromCounter_04_00(volatile std::uint64_t*, std::uint8_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteAddressFromCounterPair_04_00(volatile std::uint64_t*, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWaitOnCounter_04_00(Apr::CommandBufferObject*, std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t);
int APS5_VABI sceAmprCommandBufferWriteCounter_04_00(Apr::CommandBufferObject*, std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t, std::uint8_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWaitOnCounter_04_00(std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t);
std::uint64_t APS5_VABI sceAmprMeasureCommandSizeWriteCounter_04_00(std::uint8_t, std::uint8_t, std::uint64_t, std::uint8_t);
int APS5_VABI sceAmprCommandBufferConstructNop(Apr::CommandBufferObject*, std::int16_t, const void*, std::uint32_t, const std::uint32_t*);
int APS5_VABI sceAmprCommandBufferConstructMarker(Apr::CommandBufferObject*, std::uint32_t, const char*, const std::uint32_t*);
}

static void Require(bool value) { if (!value) std::abort(); }

namespace {

constexpr int invalidArgument = static_cast<int>(0x80020016);
constexpr int bufferFull = static_cast<int>(0x8002001C);
constexpr std::uint32_t color = 0xFF8040u;

struct Recorder {
    alignas(8) std::array<std::uint8_t, 4096> memory{};
    Apr::CommandBufferObject buffer{};
    std::uint64_t gatherState = 0;
    std::uint64_t scatterState = 0;

    explicit Recorder(std::uint32_t size = 4096) {
        Require(sceAmprCommandBufferConstructor(&buffer) == 0);
        Require(sceAmprAprCommandBufferConstructor(&buffer, &gatherState, &scatterState) == 0);
        Require(sceAmprCommandBufferSetBuffer(&buffer, memory.data(), size) == 0);
    }

    std::uint32_t Offset() const { return sceAmprCommandBufferGetCurrentOffset(&buffer); }
    std::uint32_t Commands() const { return sceAmprCommandBufferGetNumCommands(&buffer); }
};

void RequireAppended(const Recorder& recorder, std::uint32_t offset, std::uint32_t commands, Apr::Opcode opcode, std::uint64_t measured) {
    Require(recorder.Offset() == offset + measured);
    Require(recorder.Commands() == commands + 1);
    Apr::CommandHeader header;
    std::memcpy(&header, recorder.memory.data() + offset, sizeof(header));
    Require(header.opcode == opcode && header.bytes == measured);
}

void RequireRecorded(const Recorder& recorder, std::uint32_t offset, std::uint32_t commands, Apr::Opcode opcode, std::uint64_t measured, const std::string& text) {
    RequireAppended(recorder, offset, commands, opcode, measured);
    Require(measured >= sizeof(Apr::MarkerCommand) + text.size() + 1);
    Require(std::memcmp(recorder.memory.data() + offset + sizeof(Apr::MarkerCommand), text.c_str(), text.size() + 1) == 0);
}

void TestRecording(const std::string& text) {
    Recorder recorder;
    const char* marker = text.c_str();

    auto offset = recorder.Offset();
    auto commands = recorder.Commands();
    Require(sceAmprCommandBufferPushMarker(&recorder.buffer, marker) == 0);
    RequireRecorded(recorder, offset, commands, Apr::Opcode::PushMarker, sceAmprMeasureCommandSizePushMarker(marker), text);

    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferPushMarkerWithColor(&recorder.buffer, marker, color) == 0);
    RequireRecorded(recorder, offset, commands, Apr::Opcode::PushMarker, sceAmprMeasureCommandSizePushMarkerWithColor(marker, color), text);

    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferSetMarker(&recorder.buffer, marker) == 0);
    RequireRecorded(recorder, offset, commands, Apr::Opcode::SetMarker, sceAmprMeasureCommandSizeSetMarker(marker), text);

    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferSetMarkerWithColor(&recorder.buffer, marker, &color) == 0);
    RequireRecorded(recorder, offset, commands, Apr::Opcode::SetMarker, sceAmprMeasureCommandSizeSetMarkerWithColor(marker, color), text);

    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferPopMarker(&recorder.buffer) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::PopMarker, sceAmprMeasureCommandSizePopMarker());
}

void TestRejectedArguments() {
    Recorder recorder;
    Require(sceAmprCommandBufferPushMarker(&recorder.buffer, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferPushMarkerWithColor(&recorder.buffer, nullptr, color) == invalidArgument);
    Require(sceAmprCommandBufferSetMarker(&recorder.buffer, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferSetMarkerWithColor(&recorder.buffer, nullptr, &color) == invalidArgument);
    Require(sceAmprCommandBufferSetMarkerWithColor(&recorder.buffer, "frame", nullptr) == invalidArgument);
    Require(sceAmprCommandBufferPushMarker(nullptr, "frame") == invalidArgument);
    Require(sceAmprCommandBufferPushMarkerWithColor(nullptr, "frame", color) == invalidArgument);
    Require(sceAmprCommandBufferSetMarker(nullptr, "frame") == invalidArgument);
    Require(sceAmprCommandBufferSetMarkerWithColor(nullptr, "frame", &color) == invalidArgument);
    Require(sceAmprCommandBufferPopMarker(nullptr) == invalidArgument);
    Require(recorder.Offset() == 0 && recorder.Commands() == 0);

    const auto rejected = static_cast<std::uint64_t>(static_cast<std::uint32_t>(invalidArgument));
    Require(sceAmprMeasureCommandSizePushMarker(nullptr) == rejected);
    Require(sceAmprMeasureCommandSizePushMarkerWithColor(nullptr, color) == rejected);
    Require(sceAmprMeasureCommandSizeSetMarker(nullptr) == rejected);
    Require(sceAmprMeasureCommandSizeSetMarkerWithColor(nullptr, color) == rejected);
}

void TestFullBuffer() {
    const char* marker = "streaming";
    const auto measured = static_cast<std::uint32_t>(sceAmprMeasureCommandSizePushMarker(marker));
    Recorder exact(measured);
    Require(sceAmprCommandBufferPushMarker(&exact.buffer, marker) == 0);
    Require(exact.Offset() == measured && exact.Commands() == 1);
    Require(sceAmprCommandBufferPopMarker(&exact.buffer) == bufferFull);
    Require(sceAmprCommandBufferSetMarker(&exact.buffer, "") == bufferFull);
    Require(exact.Offset() == measured && exact.Commands() == 1);

    Recorder small(measured - 4);
    Require(sceAmprCommandBufferPushMarker(&small.buffer, marker) == bufferFull);
    Require(sceAmprCommandBufferSetMarkerWithColor(&small.buffer, marker, &color) == bufferFull);
    Require(small.Offset() == 0 && small.Commands() == 0);

    Apr::CommandBufferObject unbound{};
    Require(sceAmprCommandBufferConstructor(&unbound) == 0);
    Require(sceAmprCommandBufferPushMarker(&unbound, marker) == bufferFull);
    Require(sceAmprCommandBufferPopMarker(&unbound) == bufferFull);
}

void TestSubmission() {
    Recorder recorder;
    std::uint64_t first = 0;
    std::uint64_t second = 0;
    Require(sceAmprCommandBufferPushMarker(&recorder.buffer, "level") == 0);
    Require(sceAmprCommandBufferWriteAddressOnCompletion(&recorder.buffer, &first, 0x1111) == 0);
    Require(sceAmprCommandBufferSetMarkerWithColor(&recorder.buffer, "textures", &color) == 0);
    Require(sceAmprCommandBufferPushMarkerWithColor(&recorder.buffer, std::string(200, 'm').c_str(), color) == 0);
    Require(sceAmprCommandBufferSetMarker(&recorder.buffer, "") == 0);
    Require(sceAmprCommandBufferPopMarker(&recorder.buffer) == 0);
    Require(sceAmprCommandBufferPopMarker(&recorder.buffer) == 0);
    Require(sceAmprCommandBufferWriteAddressOnCompletion(&recorder.buffer, &second, 0x2222) == 0);
    Require(recorder.Commands() == 8);
    Require(sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0) == 0);
    Require(first == 0x1111 && second == 0x2222);
}

void TestWaits() {
    Recorder recorder;
    alignas(8) std::uint64_t value = 5;
    alignas(8) std::uint64_t done = 0;
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &value, 5, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &value, 3, 1, 0) == 0);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &value, 9, 2, 1) == 0);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &value, 4, 3, 0) == 0);
    Require(sceAmprCommandBufferWriteAddressOnCompletion(&recorder.buffer, &done, 1) == 0);
    auto submitted = std::async(std::launch::async, [&]() { return sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0); });
    Require(submitted.wait_for(std::chrono::seconds(10)) == std::future_status::ready);
    Require(submitted.get() == 0 && done == 1);
}

void TestCounters() {
    Recorder recorder;
    alignas(8) std::uint64_t single = 0;
    alignas(8) std::uint64_t pair = 0;
    Require(sceAmprCommandBufferWriteCounterOnCompletion(&recorder.buffer, 6, 7) == 0);
    Require(sceAmprCommandBufferWriteCounterOnCompletion(&recorder.buffer, 7, 9) == 0);
    Require(sceAmprCommandBufferWaitOnCounter(&recorder.buffer, 6, 7, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter(&recorder.buffer, 7, 8, 1, 1) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, &single, 6) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(&recorder.buffer, &pair, 6) == 0);
    Require(sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0) == 0);
    Require(single == 7 && pair == (7ull | (9ull << 32u)));
}

void TestRejectedWaitsAndCounters() {
    Recorder recorder;
    alignas(8) std::uint64_t words[2] = {};
    auto* misaligned = reinterpret_cast<volatile std::uint64_t*>(reinterpret_cast<std::uint8_t*>(words) + 4);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &words[0], 0, 4, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, &words[0], 0, 0, 2) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnAddress(&recorder.buffer, misaligned, 0, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter(&recorder.buffer, 128, 0, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter(&recorder.buffer, 0, 0, 4, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter(&recorder.buffer, 0, 0, 0, 2) == invalidArgument);
    Require(sceAmprCommandBufferWriteCounterOnCompletion(&recorder.buffer, 128, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, &words[0], 128) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(&recorder.buffer, &words[0], 128) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(&recorder.buffer, &words[0], 7) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, nullptr, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, misaligned, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromTimeCounterOnCompletion(&recorder.buffer, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressOnCompletion(&recorder.buffer, misaligned, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteKernelEventQueueOnCompletion(&recorder.buffer, 0, 1, 0) == invalidArgument);
    Require(recorder.Offset() == 0 && recorder.Commands() == 0);
}

void TestNops() {
    Recorder recorder;
    const std::uint32_t data[3] = {0x11111111u, 0x22222222u, 0x33333333u};
    for (std::uint32_t dwords = 1; dwords <= 16; ++dwords) {
        const auto offset = recorder.Offset();
        const auto commands = recorder.Commands();
        Require(sceAmprCommandBufferNop(&recorder.buffer, dwords) == 0);
        RequireAppended(recorder, offset, commands, Apr::Opcode::Nop, sceAmprMeasureCommandSizeNop(dwords));
    }
    const auto offset = recorder.Offset();
    const auto commands = recorder.Commands();
    Require(sceAmprCommandBufferNopWithData(&recorder.buffer, 3, data) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::Nop, sceAmprMeasureCommandSizeNopWithData(4));
    Require(std::memcmp(recorder.memory.data() + offset + sizeof(Apr::CommandHeader), data, sizeof(data)) == 0);
    Require(sceAmprCommandBufferNopWithData(&recorder.buffer, 0, nullptr) == 0);
    Require(sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0) == 0);

    const auto rejected = static_cast<std::uint64_t>(static_cast<std::uint32_t>(invalidArgument));
    Require(sceAmprCommandBufferNop(&recorder.buffer, 0) == invalidArgument);
    Require(sceAmprCommandBufferNop(&recorder.buffer, 17) == invalidArgument);
    Require(sceAmprCommandBufferNopWithData(&recorder.buffer, 16, data) == invalidArgument);
    Require(sceAmprMeasureCommandSizeNop(0) == rejected && sceAmprMeasureCommandSizeNop(17) == rejected);
    Require(sceAmprMeasureCommandSizeNopWithData(0) == rejected && sceAmprMeasureCommandSizeNopWithData(17) == rejected);
}

void TestVersionedCommands() {
    Recorder recorder;
    alignas(8) std::uint64_t value = 0x8000000000000005ull;
    alignas(8) std::uint64_t single = 0;
    alignas(8) std::uint64_t pair = 0;
    alignas(8) std::uint64_t time = 0;
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&recorder.buffer, &value, 0x8000000000000003ull, 4, 0) == 0);
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&recorder.buffer, &value, 1, 6, 1) == 0);
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&recorder.buffer, &value, 0x8000000000000000ull, 5, 0) == 0);
    Require(sceAmprCommandBufferWriteCounterOnCompletion(&recorder.buffer, 10, 3) == 0);
    Require(sceAmprCommandBufferWriteCounterOnCompletion(&recorder.buffer, 11, 4) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounter_04_00(&recorder.buffer, &single, 10, 1) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterPair_04_00(&recorder.buffer, &pair, 10, 0) == 0);
    Require(sceAmprCommandBufferWriteAddressFromTimeCounter_04_00(&recorder.buffer, &time, 1) == 0);
    auto submitted = std::async(std::launch::async, [&]() { return sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0); });
    Require(submitted.wait_for(std::chrono::seconds(10)) == std::future_status::ready);
    Require(submitted.get() == 0 && single == 3 && pair == (3ull | (4ull << 32u)) && time != 0);

    const auto rejected = static_cast<std::uint64_t>(static_cast<std::uint32_t>(invalidArgument));
    Recorder empty;
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&empty.buffer, nullptr, 0, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&empty.buffer, &value, 0, 7, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnAddress_04_00(&empty.buffer, &value, 0, 0, 2) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromCounterPair_04_00(&empty.buffer, &pair, 11, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteAddressFromTimeCounter_04_00(&empty.buffer, nullptr, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteKernelEventQueue_04_00(&empty.buffer, 0, 1, 0, 0) == invalidArgument);
    Require(empty.Offset() == 0 && empty.Commands() == 0);
    Require(sceAmprMeasureCommandSizeWaitOnAddress_04_00(nullptr, 0, 0, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWaitOnAddress_04_00(&value, 0, 7, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWriteAddressFromTimeCounter_04_00(nullptr) == rejected);
    Require(sceAmprMeasureCommandSizeWriteAddressFromCounter_04_00(&single, 128) == sizeof(Apr::WriteAddressFromCounterCommand));
    Require(sceAmprMeasureCommandSizeWriteAddressFromCounter_04_00(nullptr, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWriteAddressFromCounterPair_04_00(&pair, 128) == sizeof(Apr::WriteAddressFromCounterCommand));
    Require(sceAmprMeasureCommandSizeWriteAddressFromCounterPair_04_00(&pair, 11) == rejected);
    Require(sceAmprMeasureCommandSizeWaitOnAddress_04_00(&value, 0, 6, 1) == sizeof(Apr::WaitCommand));
    Require(sceAmprMeasureCommandSizeWriteAddressFromCounterPair_04_00(&pair, 10) == sizeof(Apr::WriteAddressFromCounterCommand));
}

void SubmitWithin10Seconds(const Recorder& recorder) {
    auto submitted = std::async(std::launch::async, [&]() { return sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0); });
    Require(submitted.wait_for(std::chrono::seconds(10)) == std::future_status::ready);
    Require(submitted.get() == 0);
}

void TestVersionedCounters() {
    enum : std::uint8_t { size8, size4, size2Offset0, size2Offset1, size1Offset0, size1Offset1, size1Offset2, size1Offset3 };
    enum : std::uint8_t { store, atomicOr, atomicAndComplement, atomicXor, atomicAdd };
    Recorder recorder;
    alignas(8) std::uint64_t fields = 0;
    alignas(8) std::uint64_t wide = 0;
    alignas(8) std::uint64_t bits = 0;
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 20, size4, 0x11223344u, store, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 20, size1Offset2, 0x1AAu, store, 1) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 20, size2Offset0, 0xFFFFu, atomicAdd, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 22, size8, 0x0000000500000001ull, store, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 22, size8, 0xFFFFFFFFu, atomicAdd, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 24, size4, 0xF0u, store, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 24, size4, 0x0Fu, atomicOr, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 24, size4, 0x3Cu, atomicAndComplement, 0) == 0);
    Require(sceAmprCommandBufferWriteCounter_04_00(&recorder.buffer, 24, size4, 0xFFu, atomicXor, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 20, size1Offset3, 0x11u, 0, 0, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 20, size1Offset2, 1u, 6, 0, 0, 1) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 20, size2Offset0, 0xF000u, 4, 0, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 20, size2Offset1, 0x11ABu, 2, 0, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 22, size8, 0x0000000600000000ull, 0, 0, 0, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 24, size4, 0xFCu, 0, 1, 0x0Fu, 0) == 0);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&recorder.buffer, 24, size1Offset0, 0x3Cu, 0, 0, 0x0Fu, 0) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, &fields, 20) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterPairOnCompletion(&recorder.buffer, &wide, 22) == 0);
    Require(sceAmprCommandBufferWriteAddressFromCounterOnCompletion(&recorder.buffer, &bits, 24) == 0);
    Require(recorder.Commands() == 19);
    SubmitWithin10Seconds(recorder);
    Require(fields == 0x11AA3343u && wide == 0x0000000600000000ull && bits == 0x3Cu);

    const auto rejected = static_cast<std::uint64_t>(static_cast<std::uint32_t>(invalidArgument));
    Recorder empty;
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&empty.buffer, 0, 8, 0, 0, 0, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&empty.buffer, 0, size4, 0, 7, 0, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&empty.buffer, 0, size4, 0, 0, 2, 0, 0) == invalidArgument);
    Require(sceAmprCommandBufferWaitOnCounter_04_00(&empty.buffer, 0, size4, 0, 0, 0, 0, 2) == invalidArgument);
    Require(sceAmprCommandBufferWriteCounter_04_00(&empty.buffer, 128, size4, 0, store, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteCounter_04_00(&empty.buffer, 0, 8, 0, store, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteCounter_04_00(&empty.buffer, 0, size4, 0, 5, 0) == invalidArgument);
    Require(sceAmprCommandBufferWriteCounter_04_00(nullptr, 0, size4, 0, store, 0) == invalidArgument);
    Require(empty.Offset() == 0 && empty.Commands() == 0);
    Require(sceAmprMeasureCommandSizeWaitOnCounter_04_00(200, size1Offset3, 0, 6, 1, 0, 1) == sizeof(Apr::WaitCommand));
    Require(sceAmprMeasureCommandSizeWaitOnCounter_04_00(0, 8, 0, 0, 0, 0, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWaitOnCounter_04_00(0, size4, 0, 7, 0, 0, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWaitOnCounter_04_00(0, size4, 0, 0, 2, 0, 0) == rejected);
    Require(sceAmprMeasureCommandSizeWaitOnCounter_04_00(0, size4, 0, 0, 0, 0, 2) == rejected);
    Require(sceAmprMeasureCommandSizeWriteCounter_04_00(127, size8, 0, atomicAdd) == sizeof(Apr::WriteCounterCommand));
    Require(sceAmprMeasureCommandSizeWriteCounter_04_00(128, size4, 0, store) == rejected);
    Require(sceAmprMeasureCommandSizeWriteCounter_04_00(0, 8, 0, store) == rejected);
    Require(sceAmprMeasureCommandSizeWriteCounter_04_00(0, size4, 0, 5) == rejected);
}

void TestConstructed() {
    Recorder recorder;
    const std::uint8_t payload[5] = {1, 2, 3, 4, 5};
    const std::uint32_t word = 0xCAFEF00Du;
    auto offset = recorder.Offset();
    auto commands = recorder.Commands();
    Require(sceAmprCommandBufferConstructNop(&recorder.buffer, 7, payload, sizeof(payload), &word) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::Nop, sceAmprMeasureCommandSizeNopWithData(4));
    const std::uint8_t* data = recorder.memory.data() + offset + sizeof(Apr::CommandHeader);
    const std::uint8_t padding[3] = {};
    Require(std::memcmp(data, &word, sizeof(word)) == 0 && std::memcmp(data + 4, payload, sizeof(payload)) == 0 && std::memcmp(data + 9, padding, sizeof(padding)) == 0);

    const std::array<std::uint8_t, 60> large{};
    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferConstructNop(&recorder.buffer, 0, large.data(), 60, nullptr) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::Nop, sceAmprMeasureCommandSizeNopWithData(16));
    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferConstructNop(&recorder.buffer, 0, nullptr, 0, nullptr) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::Nop, sceAmprMeasureCommandSizeNopWithData(1));

    const std::pair<std::uint32_t, Apr::Opcode> markers[] = {{1, Apr::Opcode::SetMarker}, {2, Apr::Opcode::PushMarker}, {5, Apr::Opcode::SetMarker}, {6, Apr::Opcode::PushMarker}};
    for (const auto& [type, opcode] : markers) {
        offset = recorder.Offset();
        commands = recorder.Commands();
        Require(sceAmprCommandBufferConstructMarker(&recorder.buffer, type, "stream", &color) == 0);
        RequireRecorded(recorder, offset, commands, opcode, sceAmprMeasureCommandSizeSetMarker("stream"), "stream");
    }
    offset = recorder.Offset();
    commands = recorder.Commands();
    Require(sceAmprCommandBufferConstructMarker(&recorder.buffer, 3, nullptr, nullptr) == 0);
    RequireAppended(recorder, offset, commands, Apr::Opcode::PopMarker, sceAmprMeasureCommandSizePopMarker());
    Require(sceKernelAprSubmitCommandBuffer(&recorder.buffer, 0) == 0);

    Recorder empty;
    Require(sceAmprCommandBufferConstructNop(&empty.buffer, 0, large.data(), 61, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferConstructNop(&empty.buffer, 0, large.data(), 57, &word) == invalidArgument);
    Require(sceAmprCommandBufferConstructNop(nullptr, 0, large.data(), 4, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferConstructMarker(&empty.buffer, 5, "stream", nullptr) == invalidArgument);
    Require(sceAmprCommandBufferConstructMarker(&empty.buffer, 6, "stream", nullptr) == invalidArgument);
    Require(sceAmprCommandBufferConstructMarker(&empty.buffer, 1, nullptr, nullptr) == invalidArgument);
    Require(sceAmprCommandBufferConstructMarker(&empty.buffer, 0, "stream", &color) == invalidArgument);
    Require(sceAmprCommandBufferConstructMarker(&empty.buffer, 4, "stream", &color) == invalidArgument);
    Require(empty.Offset() == 0 && empty.Commands() == 0);
}

}

int main() {
    TestRecording("frame");
    TestRecording("");
    TestRecording("1234567");
    TestRecording("12345678");
    TestRecording(std::string(300, 'a'));
    TestRejectedArguments();
    TestFullBuffer();
    TestSubmission();
    TestWaits();
    TestCounters();
    TestRejectedWaitsAndCounters();
    TestNops();
    TestVersionedCommands();
    TestVersionedCounters();
    TestConstructed();
    return 0;
}
