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
}
int main() {
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
    return 0;
}
