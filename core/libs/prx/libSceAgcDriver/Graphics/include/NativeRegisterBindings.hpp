#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEREGISTERBINDINGS_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVEREGISTERBINDINGS_HPP
#include "SceShaders.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include <map>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace AgcDriver::Graphics {
class NativeRegisterBindings {
    struct Layout {
        std::uint64_t address;
        std::vector<ShaderRegister> records;
    };
    struct Source {
        std::shared_ptr<const Layout> layout;
        std::size_t index;
        std::uint32_t value = 0;
    };
public:
    bool Empty() const { return sources.empty(); }
    bool HasIndirect() const {
        for (const auto& [offset, source] : sources) if (source.layout) return true;
        return false;
    }
    void Write(std::uint32_t offset, std::uint32_t value) {
        sources[offset] = {nullptr, 0, value};
    }
    void Bind(const volatile ShaderRegister* registers, std::uint32_t count) {
        if (count > 0x3fffu) throw std::invalid_argument("native AGC: register binding count exceeds 14 bits");
        const auto address = reinterpret_cast<std::uintptr_t>(registers);
        GuestMemory::CheckRange(const_cast<const ShaderRegister*>(registers), count * sizeof(ShaderRegister), 4);
        if (count == 0) return;
        auto layout = std::make_shared<Layout>();
        layout->address = address;
        layout->records.resize(count);
        GuestMemory::Read(address, std::as_writable_bytes(std::span(layout->records)), 4);
        auto updated = *this;
        for (std::size_t i = 0; i < count; ++i) {
            const auto offset = layout->records[i].offset;
            if (offset > 0xffffu) throw std::invalid_argument("native AGC: invalid register offset");
            updated.sources[offset] = {layout, i};
        }
        *this = std::move(updated);
    }
    void Merge(const NativeRegisterBindings& bindings) {
        auto updated = *this;
        for (const auto& [offset, source] : bindings.sources)
            updated.sources[offset] = source;
        *this = std::move(updated);
    }
    std::map<std::uint32_t, std::uint32_t> Resolve() const {
        std::map<const Layout*, std::vector<ShaderRegister>> values;
        for (const auto& [offset, source] : sources) {
            const auto* layout = source.layout.get();
            if (!layout || values.contains(layout)) continue;
            auto& records = values[layout];
            records.resize(layout->records.size());
            GuestMemory::Read(layout->address, std::as_writable_bytes(std::span(records)), 4);
            for (std::size_t i = 0; i < records.size(); ++i)
                if (records[i].offset != layout->records[i].offset)
                    throw std::runtime_error("native AGC: mutated register binding layout requires native lowering");
        }
        std::map<std::uint32_t, std::uint32_t> result;
        for (const auto& [offset, source] : sources)
            result.emplace(offset, source.layout ? values.at(source.layout.get())[source.index].value : source.value);
        return result;
    }
private:
    std::map<std::uint32_t, Source> sources;
};
}
#endif
