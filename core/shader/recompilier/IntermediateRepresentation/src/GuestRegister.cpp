#include <IntermediateRepresentation/GuestRegister.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

bool operator==(const GuestRegister& lhs, const GuestRegister& rhs) {
    throw std::runtime_error("operator== not implemented");
}
bool operator<(const GuestRegister& lhs, const GuestRegister& rhs) {
    throw std::runtime_error("operator< not implemented");
}

}
