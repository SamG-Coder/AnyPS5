#ifndef CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_CONSTANTFOLDER_HPP
#define CORE_SHADER_RECOMPILIER_OPTIMIZATION_INCLUDE_OPTIMIZATION_CONSTANTFOLDER_HPP

#include "IntermediateRepresentation/IrProgram.hpp"
#include "IntermediateRepresentation/IrValue.hpp"

#include <span>

namespace ShaderRecompiler {

class ConstantFolder {
public:
    void Fold(IrProgram& program) const;

    void Fold(std::span<IrBlock* const> blocks) const;

private:
    [[nodiscard]] bool tryFoldValue(IrValue& value) const;
};

}

#endif
