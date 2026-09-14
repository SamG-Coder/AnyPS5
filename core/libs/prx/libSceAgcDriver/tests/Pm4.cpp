#include "prx/libSceAgcDriver/Execution/include/Pm4.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Submit/include/Dcb.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include <cstdio>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

void check(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(reason);
}

template<typename TAction>
void expectFailure(TAction action, const char* text) {
    try { action(); }
    catch (const std::runtime_error& error) {
        check(std::string(error.what()).find(text) != std::string::npos, error.what());
        return;
    }
    throw std::runtime_error("expected PM4 rejection");
}

std::vector<std::uint32_t> makePacket(std::uint32_t opcode, std::initializer_list<std::uint32_t> payload, std::uint32_t flags = 0) {
    std::vector<std::uint32_t> result{0xc0000000u | (static_cast<std::uint32_t>(payload.size() - 1) << 16u) | (opcode << 8u) | flags};
    result.insert(result.end(), payload);
    return result;
}

std::uint32_t low(const void* pointer) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pointer)); }
std::uint32_t high(const void* pointer) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pointer) >> 32u); }

void execute(AgcDriver::QueueState& state, const std::vector<std::uint32_t>& packet) {
    AgcDriver::Pm4::Validate(packet, 0);
    AgcDriver::Pm4::Execute(packet, state);
}

void testCatalog() {
    std::set<std::uint32_t> values;
    for (const auto& opcode : AgcDriver::Pm4::Opcodes) {
        check(values.insert(opcode.value).second, "duplicate PM4 opcode");
        const auto packet = makePacket(opcode.value, {0});
        check(AgcDriver::Pm4::Name(packet[0]) == opcode.name, "opcode name mismatch");
        const auto reason = AgcDriver::Pm4::UnsupportedReason(packet[0]);
        if (!reason.empty()) expectFailure([&] { AgcDriver::Pm4::Validate(packet, 0); }, std::string(reason).c_str());
    }
    check(values.size() == 54, "reference opcode catalog is incomplete");
    expectFailure([] { AgcDriver::Pm4::Validate(makePacket(0xff, {0}), 0); }, "not known");
    const std::array<std::pair<std::uint32_t, const char*>, 11> custom{{
        {5, "DRAW_RESET"}, {6, "WAIT_FLIP_DONE"}, {9, "DISPATCH_RESET"}, {11, "PUSH_MARKER"},
        {12, "POP_MARKER"}, {20, "ACQUIRE_MEM_CUSTOM"}, {21, "WRITE_DATA_CUSTOM"}, {23, "FLIP"},
        {24, "RELEASE_MEM_CUSTOM"}, {25, "DMA_DATA_CUSTOM"}, {26, "CONTEXT_STATE"}
    }};
    for (const auto& [id, name] : custom) check(AgcDriver::Pm4::Name(makePacket(0x10, {0}, id << 2)[0]) == name, "custom opcode name mismatch");
}

void testRegisters() {
    AgcDriver::QueueState state;
    const std::array<std::uint32_t, 3> indirectOpcodes{0x9f, 0x63, 0x64};
    for (auto opcode : indirectOpcodes) {
        std::array<std::uint32_t, 6> pairs{0x10, 41, 0x11, 42, 0x10, 43};
        auto packet = makePacket(opcode, {low(pairs.data()), high(pairs.data()), 0x80000000, 3});
        execute(state, packet);
        const auto& registers = opcode == 0x9f ? state.context : opcode == 0x63 ? state.shader : state.userConfig;
        check(registers.at(0x10) == 43 && registers.at(0x11) == 42, "indirect register order or bank lost");
        pairs[0] = 0x12;
        pairs[4] = 0xffffffffu;
        expectFailure([&] { execute(state, packet); }, "sentinel");
        check(!registers.contains(0x12), "invalid indirect packet partially changed state");
        packet[1] = 0x1000;
        packet[2] = 0;
        expectFailure([&] { execute(state, packet); }, "guest");
        packet[3] = 0;
        expectFailure([&] { AgcDriver::Pm4::Validate(packet, 0); }, "control");
    }
    execute(state, makePacket(0x69, {0x11, 50, 51}));
    check(state.context.at(0x11) == 50 && state.context.at(0x12) == 51, "direct registers not sequential");
    execute(state, makePacket(0x7a, {0x10, 60}));
    check(state.userConfig.at(0x10) == 60, "uconfig index zero failed");
    execute(state, makePacket(0x7a, {0x20000243, 0x441}));
    check(state.indexType == 1 && state.userConfig.at(0x243) == 0x441, "indexed VGT_INDEX_TYPE write lost state");
    expectFailure([&] { execute(state, makePacket(0x7a, {0x10000010, 1})); }, "bank selection");
    expectFailure([&] { execute(state, makePacket(0x69, {0xffff, 1, 2})); }, "overflow");
    expectFailure([&] { AgcDriver::Pm4::Validate(makePacket(0x9f, {0, 0, 0x80000000, 0}), 0x20); }, "compute");
}

void testContextAndBases() {
    AgcDriver::QueueState state;
    execute(state, makePacket(0x69, {0x10, 17}));
    execute(state, makePacket(0x76, {0x20c, 2}));
    execute(state, makePacket(0x10, {3, 0}, 0x68));
    check(state.context.empty() && state.shader.at(0x20c) == 2, "push-clear reset wrong state");
    expectFailure([&] { execute(state, makePacket(0x10, {1, 0}, 0x68)); }, "already pushed");
    execute(state, makePacket(0x69, {0x10, 19}));
    execute(state, makePacket(0x10, {2, 0}, 0x68));
    check(state.context.at(0x10) == 17, "pop did not restore context");
    expectFailure([&] { execute(state, makePacket(0x10, {2, 0}, 0x68)); }, "not been pushed");
    alignas(8) std::array<std::uint32_t, 4> arguments{7, 8, 9, 0};
    execute(state, makePacket(0x11, {1, low(arguments.data()), high(arguments.data())}, 2));
    auto packet = makePacket(0x16, {0, 0x8041});
    AgcDriver::Pm4::Validate(packet, 0);
    auto resolved = AgcDriver::Pm4::ResolveDispatch(packet, state);
    check(resolved == std::array<std::uint32_t, 5>{0xc0031500, 7, 8, 9, 0x8041}, "base-relative dispatch arguments changed");
    packet = makePacket(0x16, {low(arguments.data()), high(arguments.data()), 0x41});
    AgcDriver::Pm4::Validate(packet, 0x20);
    check(AgcDriver::Pm4::ResolveDispatch(packet, state)[3] == 9, "absolute indirect dispatch arguments changed");
    execute(state, makePacket(0x13, {32}));
    execute(state, makePacket(0x26, {0x1000, 1}));
    execute(state, makePacket(0x2a, {1}));
    execute(state, makePacket(0x2f, {3}));
    check(state.indexBufferSize == 32 && state.indexBase == 0x100001000ull && state.indexType == 1 && state.instanceCount == 3, "draw setup state lost");
    execute(state, makePacket(0x10, {0x00636261}, 0x2c));
    check(state.markers.back() == "abc", "marker text lost");
    execute(state, makePacket(0x10, {0}, 0x30));
    expectFailure([&] { execute(state, makePacket(0x10, {0}, 0x30)); }, "underflow");
    execute(state, makePacket(0x10, {0}, 0x24));
    check(state.shader.empty() && state.context.empty() && state.dispatchIndirectBase == 0 && state.indexBase == 0 && !state.savedContext, "dispatch reset retained state");
}

void testIndexedDraw() {
    AgcDriver::QueueState state;
    alignas(4) std::array<std::uint32_t, 8> indices{};
    state.indexBase = reinterpret_cast<std::uintptr_t>(indices.data());
    state.instanceCount = 3;
    const auto packet = makePacket(0x35, {4, 2, 4, 0x20});
    for (std::uint32_t type = 0; type < 3; ++type) {
        state.indexType = type;
        const auto draw = AgcDriver::Pm4::ResolveDraw(packet, state);
        const auto size = type == 0 ? 2u : type == 1 ? 4u : 1u;
        check(draw.indexAddress == state.indexBase + 2 * size && draw.indexSize == size && draw.indexCount == 4 && draw.instanceCount == 3 && draw.flags == 0x20, "indexed draw state mismatch");
    }
    expectFailure([&] { AgcDriver::Pm4::Validate(packet, 0x20); }, "compute queue");
    expectFailure([] { AgcDriver::Pm4::Validate(makePacket(0x35, {3, 0, 4, 0}), 0); }, "maximum index size");
    expectFailure([] { AgcDriver::Pm4::Validate(makePacket(0x35, {4, 0, 4, 1}), 0); }, "draw flags");
    expectFailure([] { AgcDriver::Pm4::Validate(makePacket(0x35, {4, 0, 4}), 0); }, "packet size");
    state.indexType = 3;
    expectFailure([&] { AgcDriver::Pm4::ResolveDraw(packet, state); }, "index type");
    state.indexType = 1;
    state.indexBase += 1;
    expectFailure([&] { AgcDriver::Pm4::ResolveDraw(packet, state); }, "misaligned index base");
    state.indexBase = std::numeric_limits<std::uint64_t>::max() - 3;
    expectFailure([&] { AgcDriver::Pm4::ResolveDraw(packet, state); }, "index address overflow");
    state.indexType = 2;
    state.indexBase = std::numeric_limits<std::uint64_t>::max() - 4;
    expectFailure([&] { AgcDriver::Pm4::ResolveDraw(packet, state); }, "address range overflow");
    state.indexBase = 0x1000;
    expectFailure([&] { AgcDriver::Pm4::ResolveDraw(packet, state); }, "guest");
}

void testMemory() {
    AgcDriver::QueueState state;
    std::array<std::uint32_t, 4> data{0, 0, 0, 0};
    execute(state, makePacket(0x37, {0x100, low(data.data()), high(data.data()), 11, 12}));
    check(data[0] == 11 && data[1] == 12, "WRITE_DATA increment failed");
    execute(state, makePacket(0x37, {0x10100, low(data.data()), high(data.data()), 21, 22}));
    check(data[0] == 22 && data[1] == 12, "WRITE_DATA fixed destination failed");
    execute(state, makePacket(0x81, {4, 31, 32}));
    execute(state, makePacket(0x83, {4, 2, low(data.data()), high(data.data())}));
    check(data[0] == 31 && data[1] == 32, "constant RAM round trip failed");
    expectFailure([&] { execute(state, makePacket(0x81, {0xbffc, 1, 2})); }, "overflow");
    expectFailure([&] { execute(state, makePacket(0x40, {0x10105, 0, 0, low(data.data()), high(data.data())})); }, "64-bit immediate");
}

void testCopies() {
    AgcDriver::QueueState state;
    alignas(8) std::array<std::uint32_t, 4> source{11, 12, 13, 14};
    alignas(8) std::array<std::uint32_t, 4> destination{};
    execute(state, makePacket(0x40, {0x10101, low(source.data()), high(source.data()), low(destination.data()), high(destination.data())}));
    check(destination[0] == 11 && destination[1] == 12 && destination[2] == 0, "64-bit COPY_DATA failed");
    execute(state, makePacket(0x40, {0x105, 0x12345678, 0, low(destination.data()), high(destination.data())}));
    check(destination[0] == 0x12345678, "immediate COPY_DATA failed");
    execute(state, makePacket(0x50, {0x60000000, low(source.data()), high(source.data()), low(destination.data()), high(destination.data()), 16}));
    check(source == destination, "DMA_DATA copy failed");
    execute(state, makePacket(0x50, {0x40000000, 0x44332211, 0, low(destination.data()), high(destination.data()), 6}));
    check(destination[0] == 0x44332211 && destination[1] == 0x00002211, "DMA_DATA byte fill failed");
    expectFailure([&] { execute(state, makePacket(0x50, {0x60100000, low(source.data()), high(source.data()), 0, 0, 4})); }, "GDS");
    expectFailure([&] { execute(state, makePacket(0x37, {0x100, 0x1000, 0, 1})); }, "guest");
#ifdef _WIN32
    auto* memory = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    check(memory != nullptr, "VirtualAlloc failed");
    DWORD previous = 0;
    check(VirtualProtect(memory, 4096, PAGE_READONLY, &previous) != 0, "VirtualProtect failed");
    try {
        expectFailure([&] { execute(state, makePacket(0x37, {0x100, low(memory), high(memory), 1})); }, "write permission");
    } catch (...) { VirtualFree(memory, 0, MEM_RELEASE); throw; }
    check(VirtualFree(memory, 0, MEM_RELEASE) != 0, "VirtualFree failed");
#endif
}

void testDriverSubmission() {
    std::array<std::uint32_t, 2> source{0x10, 73};
    std::array<std::uint32_t, 1> destination{};
    std::vector<std::uint32_t> commands;
    for (const auto& packet : {
        makePacket(0x9f, {low(source.data()), high(source.data()), 0x80000000, 1}),
        makePacket(0x81, {0, 83}),
        makePacket(0x42, {0}),
        makePacket(0x83, {0, 1, low(destination.data()), high(destination.data())})
    }) commands.insert(commands.end(), packet.begin(), packet.end());
    Packet packet{commands.data(), static_cast<std::uint32_t>(commands.size()), 0, {}};
    check(sceAgcDriverSubmitDcb(&packet) == 0, "PM4 submission failed");
    AgcDriverWaitIdle_nid_postfix();
    check(destination[0] == 83, "worker did not execute PM4 memory operations");
    destination[0] = 0;
    const auto draw = makePacket(0x2d, {3, 2});
    commands.insert(commands.end(), draw.begin(), draw.end());
    packet = Packet{commands.data(), static_cast<std::uint32_t>(commands.size()), 0, {}};
    expectFailure([&] { sceAgcDriverSubmitDcb(&packet); }, "DRAW_INDEX_AUTO at DWORD");
    AgcDriverWaitIdle_nid_postfix();
    check(destination[0] == 0, "rejected submission executed a prefix");
}

void testAsyncMemoryFailure() {
    auto commands = makePacket(0x37, {0x100, 0x1000, 0, 1});
    Packet packet{commands.data(), static_cast<std::uint32_t>(commands.size()), 0, {}};
    check(sceAgcDriverSubmitDcb(&packet) == 0, "memory packet was not submitted");
    expectFailure([] { AgcDriverWaitIdle_nid_postfix(); }, "guest");
    expectFailure([] { AgcDriverSuspendPoint_nid_postfix(); }, "guest");
    expectFailure([&] { sceAgcDriverSubmitDcb(&packet); }, "guest");
    expectFailure([] { LibcRunShutdown_nid_postfix(); }, "guest");
}

}

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "failure") {
            testAsyncMemoryFailure();
            std::puts("PM4 asynchronous memory failure propagated to idle, suspend, submit and shutdown");
            return 0;
        }
        testCatalog();
        testRegisters();
        testContextAndBases();
        testIndexedDraw();
        testMemory();
        testCopies();
        testDriverSubmission();
        LibcRunShutdown_nid_postfix();
        std::puts("PM4 catalog, registers, state, memory and submission tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        try { LibcRunShutdown_nid_postfix(); } catch (...) {}
        return 1;
    }
}
