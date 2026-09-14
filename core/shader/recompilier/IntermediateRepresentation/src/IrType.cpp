#include "IntermediateRepresentation/IrType.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

bool IsFloatType(IrType type) {
    throw std::runtime_error("IsFloatType not implemented");
}
bool IsIntegerType(IrType type) {
    throw std::runtime_error("IsIntegerType not implemented");
}
bool IsVectorType(IrType type) {
    throw std::runtime_error("IsVectorType not implemented");
}
std::uint32_t ComponentCount(IrType type) {
    throw std::runtime_error("ComponentCount not implemented");
}
std::string TypeToString(IrType type) {
    throw std::runtime_error("TypeToString not implemented");
}

IrType operator|(IrType lhs, IrType rhs) {
    throw std::runtime_error("operator| not implemented");
}

IrType operator&(IrType lhs, IrType rhs) {
    throw std::runtime_error("operator& not implemented");
}

bool TypesOverlap(IrType lhs, IrType rhs) {
    throw std::runtime_error("TypesOverlap not implemented");
}

bool AreTypesCompatible(IrType lhs, IrType rhs) {
    throw std::runtime_error("AreTypesCompatible not implemented");
}

}
