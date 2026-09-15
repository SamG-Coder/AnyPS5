#include "IntermediateRepresentation/GuestRegister.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

bool operator==(const GuestRegister& lhs, const GuestRegister& rhs) {
    throw std::runtime_error("operator== not implemented");
}
bool operator<(const GuestRegister& lhs, const GuestRegister& rhs) {
    throw std::runtime_error("operator< not implemented");
}

std::uint32_t RegIndex(ScalarReg reg) {
    throw std::runtime_error("RegIndex not implemented");
}

std::uint32_t RegIndex(VectorReg reg) {
    throw std::runtime_error("RegIndex not implemented");
}

}
