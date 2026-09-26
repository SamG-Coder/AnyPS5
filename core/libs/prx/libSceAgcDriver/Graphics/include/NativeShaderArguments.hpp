#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVESHADERARGUMENTS_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_NATIVESHADERARGUMENTS_HPP
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace AgcDriver::Graphics {
struct NativeArgumentSnapshot {
    std::vector<std::uint32_t> values;
    std::uint64_t missing = 0;
};

// Native shader argument storage. Vertex and fragment stages own independent
// instances. This stores data, not instructions or a replayable register stream.
class NativeShaderArguments {
public:
    void SetCount(std::uint32_t value) {
        if (value > values.size()) throw std::invalid_argument("native shader argument count exceeds 32");
        count = value;
    }
    void Write(std::uint32_t first, std::span<const std::uint32_t> data) {
        if (first > values.size() || data.size() > values.size() - first)
            throw std::invalid_argument("native shader argument write exceeds stage storage");
        for (std::size_t i = 0; i < data.size(); ++i) {
            values[first + i] = data[i];
            written |= std::uint64_t{1} << (first + i);
        }
    }
    NativeArgumentSnapshot Capture() const {
        if (!count) throw std::runtime_error("native shader argument count has not been declared");
        const auto mask = (std::uint64_t{1} << *count) - 1;
        return {{values.begin(), values.begin() + *count}, mask & ~written};
    }
private:
    std::array<std::uint32_t, 32> values{};
    std::uint64_t written = 0;
    std::optional<std::uint32_t> count;
};
}
#endif
