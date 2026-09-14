#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRTYPE_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IRTYPE_HPP

#include <cstdint>
#include <string>

namespace ShaderRecompiler {

enum class IrType {
    Void,
    Bool,
    U32,
    S32,
    F32,
    U64,
    S64,
    F64,
    Vec2U32,
    Vec3U32,
    Vec4U32,
    Vec2F32,
    Vec3F32,
    Vec4F32,
    Label
};

[[nodiscard]] bool IsFloatType(IrType type);
[[nodiscard]] bool IsIntegerType(IrType type);
[[nodiscard]] bool IsVectorType(IrType type);
[[nodiscard]] std::uint32_t ComponentCount(IrType type);
[[nodiscard]] std::string TypeToString(IrType type);

}

#endif
