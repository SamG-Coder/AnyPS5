#include "prx/libSceAgcDriver/Graphics/include/NativeShaderArguments.hpp"
#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include <array>
#include <stdexcept>

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
    CommandBuffer nested{nestedWords.data(), nestedWords.data() + nestedWords.size(),
        nestedWords.data(), nestedWords.data() + nestedWords.size(), nullptr, nullptr, 0};
};
bool APS5_VABI allocate(CommandBuffer* buffer, std::uint32_t count, void* userData) {
    auto& allocation = *static_cast<Allocation*>(userData);
    allocation.requested = count;
    aps5NativeAgcSetIndexCount(&allocation.nested, 7);
    if (!allocation.success) return false;
    buffer->bottom = allocation.words.data();
    buffer->top = allocation.words.data() + allocation.words.size();
    buffer->cursor_up = buffer->bottom;
    buffer->cursor_down = allocation.insufficient ? buffer->bottom + 1 : buffer->top;
    return true;
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
        check(words[i] == (i < 10 ? 0u : 0xabcdef01u));
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
int main() {
    scalarStorage();
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
