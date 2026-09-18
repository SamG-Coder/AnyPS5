#ifndef CORE_SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_INCLUDE_INTERMEDIATEREPRESENTATION_IRMETADATA_DESCRIPTORBINDING_HPP
#define CORE_SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_INCLUDE_INTERMEDIATEREPRESENTATION_IRMETADATA_DESCRIPTORBINDING_HPP

#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace ShaderRecompiler {

inline constexpr std::uint32_t FirstImageBinding = 1u;
inline constexpr std::uint32_t FirstComparisonImageBinding = 22u;
inline constexpr std::uint32_t FirstStorageImageBinding = 29u;
inline constexpr std::uint32_t ImageBindingCount = 43u;

enum class DescriptorBindingKind : std::uint32_t {
    Buffers = 0u,
    Samplers = FirstImageBinding + ImageBindingCount,
    Gds,
    BdaPagetable,
    FaultBuffer,
    FlattenedSrt,
    ShaderData,
    Count,
};

struct PushData {
    static constexpr std::uint32_t DwordCount = 32;
    static constexpr std::uint32_t MeshDrawDwordCount = 6;
    static constexpr std::uint32_t NoStart = std::numeric_limits<std::uint32_t>::max();
    std::array<std::uint32_t, DwordCount> dwords {};

    [[nodiscard]] static bool CanFit(std::uint32_t start, std::uint32_t size) {
        throw std::runtime_error("CanFit not implemented");
    }
    [[nodiscard]] static std::uint32_t StartFor(std::uint32_t cursor, std::uint32_t size) {
        throw std::runtime_error("StartFor not implemented");
    }
};

struct IrDescriptorBinding {
    DescriptorBindingKind kind = DescriptorBindingKind::Buffers;
    std::vector<std::uint32_t> resources;

    bool operator==(const IrDescriptorBinding& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

struct IrBindingLayout {
    std::uint32_t pushDataStartDword = PushData::NoStart;
    std::uint32_t memoryOffsetDword = 0;
    std::uint32_t memoryOffsetCount = 0;
    std::vector<std::uint32_t> userDataRegisters;
    std::vector<IrDescriptorBinding> descriptors;

    [[nodiscard]] std::uint32_t ShaderDataDwords() const {
        throw std::runtime_error("ShaderDataDwords not implemented");
    }
    [[nodiscard]] bool UsesPushData() const {
        throw std::runtime_error("UsesPushData not implemented");
    }
    void AdvancePushData(std::uint32_t& cursor) const {
        throw std::runtime_error("AdvancePushData not implemented");
    }

    bool operator==(const IrBindingLayout& other) const {
        throw std::runtime_error("operator== not implemented");
    }
};

}

#endif
