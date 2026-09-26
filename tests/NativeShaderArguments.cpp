#include "prx/libSceAgcDriver/Graphics/include/NativeShaderArguments.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeGraphicsRuntime.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeRegisterBindings.hpp"
#include "prx/libSceAgcDriver/Graphics/include/NativeGraphicsState.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include <thread>
#include <functional>
#include <array>
#include <stdexcept>
#include <string>

namespace {
void check(bool value) { if (!value) throw std::runtime_error("native shader argument regression"); }
template<class F> void rejects(F action) {
    try { action(); } catch (const std::exception&) { return; }
    throw std::runtime_error("invalid native arguments accepted");
}
struct Allocation {
    std::array<std::uint32_t, 16> words;
    std::array<std::uint32_t, 4> nestedWords{};
    std::uint32_t requested = 0;
    bool success = true;
    bool insufficient = false;
    std::uint32_t* source = nullptr;
    CommandBuffer nested{nestedWords.data(), nestedWords.data() + nestedWords.size(),
        nestedWords.data(), nestedWords.data() + nestedWords.size(), nullptr, nullptr, 0};
};
void graphicsState() {
    AgcDriver::Graphics::NativeGraphicsState graphics;
    check(!graphics.ReadyForDraw());
    graphics.SetContext(0x90, 0x800a0014);
    graphics.SetContext(0x91, 0x003c0064);
    auto scissor = graphics.Get().scissor;
    check(scissor.offset.x == 20 && scissor.offset.y == 10);
    check(scissor.extent.width == 80 && scissor.extent.height == 50);
    graphics.SetContext(0x94, 0x8014001e);
    graphics.SetContext(0x95, 0x00320050);
    scissor = graphics.Get().scissor;
    check(scissor.offset.x == 30 && scissor.offset.y == 20);
    check(scissor.extent.width == 50 && scissor.extent.height == 30);
    graphics.SetContext(0x292, 0);
    check(graphics.Get().scissor.extent.width == 80);
    graphics.SetContext(0x8e, 15);
    graphics.SetContext(0x8f, 15);
    graphics.SetContext(0x202, 0xcc0010);
    graphics.SetContext(0x318, 0x1000);
    graphics.SetContext(0x390, 0);
    graphics.SetContext(0x31c, 0x8028);
    graphics.SetContext(0x3b0, (63u << 14) | 31u);
    graphics.SetContext(0x3b8, 0x09000000);
    check(graphics.Get().renderExtent.width == 64 && graphics.Get().renderExtent.height == 32);
    scissor = graphics.Get().scissor;
    check(scissor.extent.width == 44 && scissor.extent.height == 22);
    rejects([&] { graphics.SetContext(0x200, 1); });
    rejects([&] { graphics.SetContext(0x0, 1); });
    rejects([&] { graphics.SetContext(0x31c, 0x10008028); });
    rejects([&] { graphics.SetContext(0x206, 0); });
    rejects([&] { graphics.SetContext(0x201, 1); });
    rejects([&] { graphics.SetContext(0x999, 0); });
    AgcDriver::Graphics::NativeGraphicsState shader;
    const std::array<ShaderRegister,1> metadata{{{0x1c2, 1}}};
    shader.SetContext(0x1c2, 1);
    shader.ValidateShaderContext(metadata, {});
    rejects([&] { shader.ValidateShaderContext({}, {}); });
    shader.SetContext(0x1c2, 2);
    rejects([&] { shader.ValidateShaderContext(metadata, {}); });
    shader.SetContext(0x1b6, 1);
    shader.SetContext(0x1b3, 2);
    shader.SetContext(0x1b4, 2);
    shader.SetContext(0x203, 0);
    shader.SetContext(0x1c5, 4);
    shader.SetContext(0x191, 0x400);
    check(shader.PixelStage()->interpolatorSettings[0] == 0x400);
    shader.SetContext(0x1b5, 0);
    check(shader.PixelStage()->interpolatorSettings[0] == 0);
    AgcDriver::Graphics::NativeGraphicsState viewport;
    viewport.SetUser(0x242, 4);
    viewport.SetContext(0x10f, 0x3f800000);
    viewport.SetContext(0x110, 0x3f800000);
    viewport.SetContext(0x111, 0xbf800000);
    viewport.SetContext(0x112, 0x3f800000);
    viewport.SetContext(0x113, 0x3f000000);
    viewport.SetContext(0x114, 0x3f000000);
    check(viewport.ReadyForDraw());
    check(viewport.Get().viewport.minDepth == 0 && viewport.Get().viewport.maxDepth == 1);
    viewport.SetContext(0xb4, 0xbf800000);
    viewport.SetContext(0xb5, 0x40000000);
    check(viewport.ReadyForDraw());
    viewport.SetContext(0xb5, 0x3f000000);
    rejects([&] { viewport.ReadyForDraw(); });
    rejects([&] { viewport.SetContext(0xb4, 0x7fc00000); });
}
bool APS5_VABI allocate(CommandBuffer* buffer, std::uint32_t count, void* userData) {
    auto& allocation = *static_cast<Allocation*>(userData);
    allocation.requested = count;
    if (allocation.source) *allocation.source = 0xffffffffu;
    aps5NativeAgcSetIndexCount(&allocation.nested, 7);
    if (!allocation.success) return false;
    buffer->bottom = allocation.words.data();
    buffer->top = allocation.words.data() + allocation.words.size();
    buffer->cursor_up = buffer->bottom;
    buffer->cursor_down = allocation.insufficient ? buffer->bottom + 1 : buffer->top;
    return true;
}
class RenderingDependency final : public AgcDriver::IRenderingWait {
public:
    explicit RenderingDependency(Label& marker) : marker(marker) {}
    void Wait() override {
        check(marker.value == 17);
        bool unlocked = false;
        std::thread other([&] {
            auto& mutex = AgcDriver::NativeGraphicsRuntime::Get().Mutex();
            unlocked = mutex.try_lock();
            if (unlocked) mutex.unlock();
        });
        other.join();
        check(unlocked);
        waited = true;
    }
    Label& marker;
    bool waited = false;
};
class RenderingOutput final : public AgcDriver::IVideoOutput, public std::enable_shared_from_this<RenderingOutput> {
public:
    explicit RenderingOutput(Label& marker) : dependency(std::make_shared<RenderingDependency>(marker)) {}
    std::shared_ptr<AgcDriver::IFlipRequest> Reserve(const AgcDriver::FlipInfo&) override {
        throw std::runtime_error("unexpected flip reservation");
    }
    std::shared_ptr<AgcDriver::IRenderingWait> CaptureRenderingWait(std::uint32_t index) override {
        check(index == 2 && !dependency->waited);
        ++captures;
        AgcDriverUnregisterVideoOutput_nid_postfix(77, shared_from_this());
        return dependency;
    }
    void Fail(std::exception_ptr) noexcept override {}
    std::shared_ptr<RenderingDependency> dependency;
    unsigned captures = 0;
};
void renderingDependency() {
    static std::array<std::uint32_t, 16> words{};
    auto* cursor = words.data();
    Label marker{17};
    const auto output = std::make_shared<RenderingOutput>(marker);
    AgcDriverRegisterVideoOutput_nid_postfix(77, output);
    check(aps5NativeAgcGetWaitRenderingSize() == 4);
    rejects([&] { aps5NativeAgcWaitUntilSafeForRendering(&cursor, 3, 0, 77, 2); });
    rejects([&] { aps5NativeAgcWaitUntilSafeForRendering(&cursor, 4, 1, 77, 2); });
    rejects([&] { aps5NativeAgcWaitUntilSafeForRendering(&cursor, 4, 0, 77, -1); });
    check(cursor == words.data());
    check(aps5NativeAgcWaitUntilSafeForRendering(&cursor, 4, 0, 77, 2) == 0);
    check(cursor == words.data() + 4 && output->captures == 0);
    CommandBuffer command{words.data(), words.data() + words.size(), cursor,
        words.data() + words.size(), nullptr, nullptr, 0};
    aps5NativeAgcReleaseMem(&command, 0x28, 0x30c, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    const Packet split{words.data() + 1, 11, 0, {0, 0, 0}};
    rejects([&] { aps5NativeAgcSubmit(&split); });
    check(output->captures == 0 && marker.value == 17);
    const Packet full{words.data(), 12, 0, {0, 0, 0}};
    check(aps5NativeAgcSubmit(&full) == 0);
    check(output->captures == 1 && output->dependency->waited && marker.value == 42);
}
class FlipProbe final : public AgcDriver::IFlipRequest, public AgcDriver::IRenderingWait {
public:
    explicit FlipProbe(Label& marker) : marker(marker) {}
    void Wait() override {
        check(reserved && marker.value == 17);
        if (failWait) throw std::runtime_error("flip wait failure");
        waited = true;
    }
    void GpuReady(const std::shared_ptr<AgcDriver::FrameTiming>& timing) override {
        check(timing != nullptr && waited && marker.value == 42);
        timing->Print(78, 2, 0, AgcDriver::FrameTiming::Clock::now(), {});
        bool unlocked = false;
        std::thread other([&] {
            auto& mutex = AgcDriver::NativeGraphicsRuntime::Get().Mutex();
            unlocked = mutex.try_lock();
            if (unlocked) mutex.unlock();
        });
        other.join();
        check(unlocked);
        if (failReady) throw std::runtime_error("flip preparation failure");
        ready = true;
        readyHook();
    }
    void Fail(std::exception_ptr error) noexcept override { failed = error != nullptr; }
    Label& marker;
    bool captured = false, reserved = false, waited = false, ready = false, failed = false, failWait = false, failReady = false;
    std::function<void()> readyHook;
};
class FlipOutput final : public AgcDriver::IVideoOutput {
public:
    explicit FlipOutput(Label& marker) : probe(std::make_shared<FlipProbe>(marker)) {}
    std::shared_ptr<AgcDriver::IRenderingWait> CaptureRenderingWait(std::uint32_t index) override {
        check(index == 2 && !probe->reserved);
        probe->captured = true;
        return probe;
    }
    std::shared_ptr<AgcDriver::IFlipRequest> Reserve(const AgcDriver::FlipInfo& info) override {
        check(probe->captured && info.handle == 78 && info.index == 2 && info.mode == 1 && info.argument == -0x123456789abcdefLL);
        probe->reserved = true;
        return probe;
    }
    void Fail(std::exception_ptr error) noexcept override { probe->Fail(error); }
    std::shared_ptr<FlipProbe> probe;
};
void terminalFlip(bool failWait, bool failReady = false) {
    static std::array<std::uint32_t, 32> words{};
    auto* cursor = words.data();
    Label marker{17};
    const auto output = std::make_shared<FlipOutput>(marker);
    output->probe->failWait = failWait;
    output->probe->failReady = failReady;
    output->probe->readyHook = [weak = std::weak_ptr<FlipOutput>(output)] {
        AgcDriverUnregisterVideoOutput_nid_postfix(78, weak.lock());
    };
    AgcDriverRegisterVideoOutput_nid_postfix(78, output);
    aps5NativeAgcWaitUntilSafeForRendering(&cursor, 4, 0, 78, 2);
    CommandBuffer command{words.data(), words.data() + words.size(), cursor,
        words.data() + words.size(), nullptr, nullptr, 0};
    aps5NativeAgcReleaseMem(&command, 0x28, 0x30c, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    check(aps5NativeAgcSetFlip(&command, 78, 2, 1, -0x123456789abcdefLL) == words.data() + 12);
    check(command.cursor_up == words.data() + 18 && !output->probe->reserved);
    aps5NativeAgcReleaseMem(&command, 0x28, 0, 0, 0, &marker, 1, 43, 0, 0, 0, 0);
    check(command.cursor_up == words.data() + 26);
    const Packet partial{words.data(), 18, 0, {0, 0, 0}};
    rejects([&] { aps5NativeAgcSubmit(&partial); });
    check(!output->probe->reserved && marker.value == 17);
    const Packet complete{words.data(), 26, 0, {0, 0, 0}};
    if (failWait || failReady) {
        rejects([&] { aps5NativeAgcSubmit(&complete); });
        check(output->probe->failed && !output->probe->ready && marker.value == (failWait ? 17u : 42u));
        AgcDriverUnregisterVideoOutput_nid_postfix(78, output);
    } else {
        check(aps5NativeAgcSubmit(&complete) == 0);
        check(output->probe->ready && !output->probe->failed && marker.value == 43);
    }
}
void terminalCompletion() {
    static std::array<std::uint32_t, 32> words{};
    CommandBuffer first{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    Label marker{0x1122334455667788ull};
    auto* cache = aps5NativeAgcReleaseMem(&first, 0x2d, 0xc, 1, 0, nullptr, 0, 0, 0, 1, 0, 0);
    check(cache == words.data() && first.cursor_up == words.data() + 8);
    CommandBuffer second = first;
    first = {};
    auto* release = aps5NativeAgcReleaseMem(&second, 0x28, 0x30c, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    check(release == words.data() + 8 && second.cursor_up == words.data() + 16);
    check(marker.value == 0x1122334455667788ull);
    const auto before = words;
    try {
        aps5NativeAgcDrawIndexAuto(&second, 4, 2);
        check(false);
    } catch (const std::runtime_error& error) {
        check(std::string(error.what()).find("drawing after a terminal release") != std::string::npos);
    }
    check(words == before && second.cursor_up == words.data() + 16);
    second = {};
    const Packet partial{words.data(), 8, 0, {0, 0, 0}};
    rejects([&] { aps5NativeAgcSubmit(&partial); });
    check(marker.value == 0x1122334455667788ull);
    const Packet complete{words.data(), 16, 0, {0, 0, 0}};
    check(aps5NativeAgcSubmit(&complete) == 0);
    check(marker.value == 0x112233440000002aull);
    CommandBuffer invalid{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    for (const auto select : {2u, 3u, 5u})
        rejects([&] { aps5NativeAgcReleaseMem(&invalid, 0x28, 0, 0, 0, &marker, select, 42, 0, 0, 0, 0); });
    rejects([&] { aps5NativeAgcReleaseMem(&invalid, 0x28, 0, 0, 0, nullptr, 1, 42, 0, 0, 0, 0); });
    rejects([&] { aps5NativeAgcReleaseMem(&invalid, 0x28, 0, 0, 0, &marker, 1, 1ull << 32, 0, 0, 0, 0); });
    rejects([&] { aps5NativeAgcReleaseMem(&invalid, 0x28, 0, 0, 0, &marker, 1, 42, 0, 0, 1, 0); });
    check(invalid.cursor_up == words.data() && words == before);
}
void descriptorLifetime() {
    static std::array<std::uint32_t, 16> words{};
    static CommandBuffer descriptor;
    descriptor = {words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    aps5NativeAgcSetIndexCount(&descriptor, 7);
    const Packet submission{words.data(), 2, 0, {0, 0, 0}};
    descriptor = {};
    check(aps5NativeAgcSubmit(&submission) == 0);
    check(aps5NativeAgcSubmit(&submission) == 0);
}
void contextBindings() {
    using AgcDriver::Graphics::NativeRegisterBindings;
    std::array<ShaderRegister, 3> first{{{0x205, 1}, {0x204, 2}, {0x205, 3}}};
    NativeRegisterBindings state;
    state.Bind(first.data(), first.size());
    const auto draw = state;
    std::array<ShaderRegister, 1> second{{{0x205, 4}}};
    NativeRegisterBindings replacement;
    replacement.Bind(second.data(), second.size());
    state.Merge(replacement);
    first[2].value = 5;
    second[0].value = 6;
    check(draw.Resolve().at(0x205) == 5);
    check(state.Resolve().at(0x205) == 6 && state.Resolve().at(0x204) == 2);
    auto direct = state;
    direct.Write(0x205, 11);
    check(direct.Resolve().at(0x205) == 11 && state.Resolve().at(0x205) == 6);
    NativeRegisterBindings directInput;
    directInput.Write(0x204, 12);
    direct.Merge(directInput);
    check(direct.Resolve().at(0x204) == 12);
    first[0].offset = 0x200;
    rejects([&] { state.Resolve(); });
    first[0].offset = 0x205;
    rejects([&] { state.Bind(nullptr, 1); });
    rejects([&] { state.Bind(first.data(), 0x4000); });
    rejects([&] { state.Bind(reinterpret_cast<const ShaderRegister*>(reinterpret_cast<std::uintptr_t>(first.data()) + 1), 1); });
    first[0].offset = 0x10000;
    rejects([&] { state.Bind(first.data(), 1); });
    first[0].offset = 0x205;
    check(state.Resolve().at(0x205) == 6);
    NativeRegisterBindings empty;
    empty.Bind(first.data(), 0);
    check(empty.Empty());
    static std::array<std::uint32_t, 16> words;
    words.fill(0xabcdef01);
    CommandBuffer command{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    check(aps5NativeAgcSetCxRegistersIndirect(&command, first.data(), first.size()) == words.data());
    check(command.cursor_up == words.data() + 5);
    for (std::size_t i = 0; i < words.size(); ++i) check(words[i] == (i < 5 ? 0x80000000u : 0xabcdef01u));
    command.cursor_down = command.cursor_up + 4;
    rejects([&] { aps5NativeAgcSetCxRegistersIndirect(&command, second.data(), second.size()); });
    check(command.cursor_up == words.data() + 5);
    first[0].offset = 0x10000;
    rejects([&] { aps5NativeAgcSetCxRegistersIndirect(&command, first.data(), first.size()); });
    check(command.cursor_up == words.data() + 5);
}
void deferredRegisters(bool mutateLayout) {
    static std::array<std::uint32_t, 32> words{};
    CommandBuffer command{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    ShaderRegister shader{0x8b, 0}, user{0x243, 0};
    Label marker{17};
    aps5NativeAgcReleaseMem(&command, 0x2d, 0xc, 1, 0, nullptr, 0, 0, 0, 0, 0, 0);
    aps5NativeAgcSetShRegistersIndirect(&command, &shader, 1);
    aps5NativeAgcSetUcRegistersIndirect(&command, &user, 1);
    aps5NativeAgcDrawIndex(&command, 3, reinterpret_cast<void*>(0x1000), 0);
    aps5NativeAgcReleaseMem(&command, 0x28, 0, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    check(command.cursor_up == words.data() + 32);
    if (mutateLayout) user.offset = 0x242;
    else user.value = 2;
    const Packet packet{words.data(), 32, 0, {0, 0, 0}};
    try {
        aps5NativeAgcSubmit(&packet);
        check(false);
    } catch (const std::runtime_error& error) {
        check(std::string(error.what()).find(mutateLayout ? "mutated register binding layout" : "unsupported native index size") != std::string::npos);
    }
    check(marker.value == 17);
}
void recordingRestart() {
    static std::array<std::uint32_t, 32> words{};
    CommandBuffer command{words.data(), words.data() + words.size(), words.data() + 4,
        words.data() + words.size(), nullptr, nullptr, 0};
    Label marker{17};
    aps5NativeAgcSetIndexCount(&command, 7);
    aps5NativeAgcReleaseMem(&command, 0x28, 0, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    command.cursor_up = words.data() + 6;
    rejects([&] { aps5NativeAgcSetIndexCount(&command, 9); });
    command.cursor_up = words.data() + 4;
    aps5NativeAgcSetIndexCount(&command, 9);
    const Packet packet{words.data(), 6, 0, {0, 0, 0}};
    check(aps5NativeAgcSubmit(&packet) == 0);
    check(marker.value == 17);
}
void descriptorReuse() {
    static std::array<std::uint32_t, 16> first{}, second{};
    CommandBuffer descriptor{first.data(), first.data() + first.size(), first.data(),
        first.data() + first.size(), nullptr, nullptr, 0};
    aps5NativeAgcSetIndexCount(&descriptor, 7);
    const Packet firstSubmission{first.data(), 2, 0, {0, 0, 0}};
    descriptor = {second.data(), second.data() + second.size(), second.data(),
        second.data() + second.size(), nullptr, nullptr, 0};
    aps5NativeAgcSetIndexCount(&descriptor, 11);
    const Packet secondSubmission{second.data(), 2, 0, {0, 0, 0}};
    descriptor = {};
    check(aps5NativeAgcSubmit(&firstSubmission) == 0);
    check(aps5NativeAgcSubmit(&secondSubmission) == 0);
    check(aps5NativeAgcSubmit(&firstSubmission) == 0);
}
void drawFailures() {
    std::array<std::uint32_t, 24> words;
    words.fill(0xabcdef01u);
    static CommandBuffer buffer;
    buffer = {words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    const auto original = words;
    rejects([&] { aps5NativeAgcDrawIndex(&buffer, 4, reinterpret_cast<void*>(0x1000), 0); });
    try {
        aps5NativeAgcDrawIndexOffset(&buffer, 0, 4, 0);
        check(false);
    } catch (const std::runtime_error& error) {
        check(std::string(error.what()).find("missing index buffer state") != std::string::npos);
    }
    check(buffer.cursor_up == words.data() && words == original);
    for (const auto modifier : {0ull, 2ull}) {
        try {
            aps5NativeAgcDrawIndexAuto(&buffer, 4, modifier);
            check(false);
        } catch (const std::runtime_error& error) {
            check(std::string(error.what()).find("missing native vertex or fragment shader") != std::string::npos);
        }
        check(buffer.cursor_up == words.data() && words == original);
    }
    for (unsigned kind = 0; kind < 3; ++kind) {
        Allocation allocation;
        allocation.words.fill(0xabcdef01u);
        buffer = {nullptr, nullptr, nullptr, nullptr, allocate, &allocation, 0};
        const auto draw = [&](std::uint64_t modifier) {
            if (kind == 0) aps5NativeAgcDrawIndex(&buffer, 4, reinterpret_cast<void*>(0x1000), modifier);
            else if (kind == 1) aps5NativeAgcDrawIndexAuto(&buffer, 4, modifier);
            else aps5NativeAgcDrawIndexOffset(&buffer, 0, 4, modifier);
        };
        for (const auto modifier : {1ull, 3ull, 0x100ull, 0x102ull, 0x200000000ull})
            rejects([&] { draw(modifier); });
        check(allocation.requested == 0);
        rejects([&] { draw(0); });
        check(allocation.requested == (kind == 0 ? 6u : kind == 1 ? 3u : 5u));
        check(buffer.cursor_up == allocation.words.data());
        for (const auto word : allocation.words) check(word == 0xabcdef01u);
    }
}
void rangeStorage() {
    for (const bool shader : {false, true}) {
        Allocation allocation;
        allocation.words.fill(0xabcdef01u);
        std::uint32_t value = shader ? 8u : 4u;
        allocation.source = &value;
        CommandBuffer buffer{nullptr, nullptr, nullptr, nullptr, allocate, &allocation, 0};
        const auto setter = shader ? aps5NativeAgcSetShRegisterRange : aps5NativeAgcSetUcRegisterRange;
        const auto offset = shader ? 0x8bu : 0x242u;
        check(setter(&buffer, offset, &value, 1) == allocation.words.data());
        check(allocation.requested == 3 && value == 0xffffffffu);
        check(buffer.cursor_up == allocation.words.data() + 3);
        check(allocation.words[0] == 0x80000000u && allocation.words[1] == 0x80000000u && allocation.words[2] == 0x80000000u);
        check(allocation.words[3] == 0xabcdef01u);
        const auto cursor = buffer.cursor_up;
        const auto words = allocation.words;
        rejects([&] { setter(&buffer, offset, reinterpret_cast<const std::uint32_t*>(
            reinterpret_cast<std::uintptr_t>(&value) + 1), 1); });
        check(buffer.cursor_up == cursor && allocation.words == words);
        buffer.cursor_down = cursor + 2;
        buffer.callback = nullptr;
        value = shader ? 8u : 4u;
        rejects([&] { setter(&buffer, offset, &value, 1); });
        check(buffer.cursor_up == cursor && allocation.words == words);
    }
    std::array<std::uint32_t, 16> words{};
    const std::array<std::uint32_t, 2> values{4, 1};
    CommandBuffer buffer{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    check(aps5NativeAgcSetUcRegisterRange(&buffer, 0x242, values.data(), 2) == words.data());
    check(buffer.cursor_up == words.data() + 4);
    check(aps5NativeAgcSetShRegisterRange(&buffer, 0x8c, values.data(), 2) == words.data() + 4);
    check(buffer.cursor_up == words.data() + 8);
}
void scalarStorage() {
    std::array<std::uint32_t, 16> words;
    words.fill(0xabcdef01u);
    CommandBuffer buffer{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 6};
    check(aps5NativeAgcSetIndexBuffer(&buffer, 0x1000) == words.data());
    check(aps5NativeAgcSetIndexCount(&buffer, 4) == words.data() + 3);
    check(aps5NativeAgcSetIndexSize(&buffer, 1, 3) == words.data() + 5);
    check(aps5NativeAgcSetNumInstances(&buffer, 2) == words.data() + 8);
    check(buffer.cursor_up == words.data() + 10);
    for (unsigned i = 0; i < words.size(); ++i)
        check(words[i] == (i < 10 ? 0x80000000u : 0xabcdef01u));
    rejects([&] { aps5NativeAgcSetIndexCount(&buffer, 8); });
    check(buffer.cursor_up == words.data() + 10);
    for (unsigned mode = 0; mode < 3; ++mode) {
        Allocation allocation;
        allocation.words.fill(0xabcdef01u);
        allocation.success = mode != 1;
        allocation.insufficient = mode == 2;
        CommandBuffer dynamic{nullptr, nullptr, nullptr, nullptr, allocate, &allocation, 0};
        if (mode == 0) {
            check(aps5NativeAgcSetIndexBuffer(&dynamic, 0x1000) == allocation.words.data());
            check(dynamic.cursor_up == allocation.words.data() + 3);
        } else {
            rejects([&] { aps5NativeAgcSetIndexBuffer(&dynamic, 0x1000); });
            for (const auto word : allocation.words) check(word == 0xabcdef01u);
        }
        check(allocation.requested == 3);
        check(allocation.nested.cursor_up == allocation.nestedWords.data() + 2);
    }
    Allocation allocation;
    allocation.words.fill(0xabcdef01u);
    buffer.callback = allocate;
    buffer.user_data = &allocation;
    check(aps5NativeAgcSetIndexCount(&buffer, 8) == allocation.words.data());
    check(allocation.requested == 8 && buffer.reserved_dw == 6);
    const auto cursor = buffer.cursor_up;
    rejects([&] { aps5NativeAgcSetIndexBuffer(&buffer, 3); });
    rejects([&] { aps5NativeAgcSetIndexSize(&buffer, 1, 4); });
    check(buffer.cursor_up == cursor);
}
}
int main(int argc, char** argv) {
    if (argc > 1) {
        const std::string mode = argv[1];
        if (mode == "--indirect-value" || mode == "--indirect-layout") {
            check(argc == 2);
            deferredRegisters(mode == "--indirect-layout");
            return 0;
        }
        check(argc == 2 && (mode == "--flip-failure" || mode == "--flip-ready-failure"));
        terminalFlip(mode == "--flip-failure", mode == "--flip-ready-failure");
        return 0;
    }
    graphicsState();
    recordingRestart();
    contextBindings();
    terminalFlip(false);
    renderingDependency();
    terminalCompletion();
    descriptorLifetime();
    descriptorReuse();
    drawFailures();
    scalarStorage();
    rangeStorage();
    using AgcDriver::Graphics::NativeShaderArguments;
    NativeShaderArguments vertex, fragment;
    rejects([&] { vertex.Capture(); });
    vertex.SetCount(4);
    fragment.SetCount(2);
    const std::array<std::uint32_t, 2> v{11, 22}, f{33, 44};
    vertex.Write(1, v);
    fragment.Write(0, f);
    auto before = vertex.Capture();
    check(before.values == std::vector<std::uint32_t>{0, 11, 22, 0} && before.missing == 9);
    check(fragment.Capture().values == std::vector<std::uint32_t>{33, 44} && fragment.Capture().missing == 0);
    rejects([&] { vertex.Write(31, v); });
    rejects([&] { vertex.Write(UINT32_MAX, v); });
    rejects([&] { vertex.SetCount(33); });
    check(vertex.Capture().values == before.values && vertex.Capture().missing == before.missing);
    vertex.SetCount(32);
    vertex.Write(30, v);
    check((vertex.Capture().missing & (3ull << 30)) == 0);
    fragment.SetCount(0);
    check(fragment.Capture().values.empty() && fragment.Capture().missing == 0);
    // Exercise the actual lowered API: non-contiguous UC is valid graphics
    // configuration and must never be stored as vertex/fragment constants.
    std::array<std::uint32_t, 16> words{};
    CommandBuffer buffer{words.data(), words.data() + words.size(), words.data(), words.data() + words.size(), nullptr, nullptr, 0};
    const std::array<ShaderRegister, 3> configuration{{{0x242, 4}, {0x24a, 7}, {0x24b, 0}}};
    aps5NativeAgcSetUcRegisters(&buffer, configuration.data(), configuration.size());
    const auto cursor = buffer.cursor_up;
    const std::array<ShaderRegister, 2> bad{{{0x242, 4}, {0x999, 1}}};
    rejects([&] { aps5NativeAgcSetUcRegisters(&buffer, bad.data(), bad.size()); });
    check(buffer.cursor_up == cursor);
    rejects([&] { aps5NativeAgcSetUcRegisterRange(&buffer, 0x242, nullptr, 1); });
    rejects([&] { aps5NativeAgcSetShRegisterRange(&buffer, UINT32_MAX, v.data(), 2); });
    check(buffer.cursor_up == cursor);
    const auto storage = words;
    const auto validBuffer = buffer;
    const auto rejectBuffer = [&] {
        const auto savedCursor = buffer.cursor_up;
        rejects([&] { aps5NativeAgcSetNumInstances(&buffer, 2); });
        check(buffer.cursor_up == savedCursor && words == storage);
        buffer = validBuffer;
    };
    buffer.cursor_up = words.data();
    buffer.bottom = words.data() + 1;
    rejectBuffer();
    buffer.cursor_down = words.data() + words.size();
    buffer.top = words.data() + words.size() - 1;
    rejectBuffer();
    buffer.cursor_down = words.data();
    rejectBuffer();
    buffer.cursor_up = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(words.data()) + 1);
    rejectBuffer();
    buffer.reserved_dw = static_cast<std::uint32_t>(buffer.cursor_down - buffer.cursor_up);
    rejectBuffer();
    buffer.reserved_dw = static_cast<std::uint32_t>(buffer.cursor_down - buffer.cursor_up) + 1;
    rejectBuffer();
    buffer.reserved_dw = static_cast<std::uint32_t>(buffer.cursor_down - buffer.cursor_up) - 2;
    check(aps5NativeAgcSetNumInstances(&buffer, 2) == cursor);
    check(buffer.cursor_up == cursor + 2);
    rejects([&] { aps5NativeAgcSetNumInstances(&buffer, 3); });
    check(buffer.cursor_up == cursor + 2);
    return 0;
}
