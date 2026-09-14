#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_GUESTREGISTER_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_GUESTREGISTER_HPP

#include <cstdint>

namespace ShaderRecompiler {

enum class RegisterBank {
    Scalar,
    Vector,
    VectorConditionCode,
    ScalarConditionCode,
    ExecutionMask,
    Memory
};

struct GuestRegister {
    RegisterBank bank;
    std::uint32_t index;
};

[[nodiscard]] bool operator==(const GuestRegister& lhs, const GuestRegister& rhs);
[[nodiscard]] bool operator<(const GuestRegister& lhs, const GuestRegister& rhs);

}

#endif
