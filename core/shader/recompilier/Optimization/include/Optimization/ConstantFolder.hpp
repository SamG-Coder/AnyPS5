#ifndef SHADER_RECOMPILIER_OPTIMIZATION_CONSTANTFOLDER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_CONSTANTFOLDER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <IntermediateRepresentation/IrValue.hpp>

namespace ShaderRecompiler {

class ConstantFolder {
public:
    void Fold(IrProgram& program) const;

private:
    [[nodiscard]] bool tryFoldValue(IrValue& value) const;
};

}

#endif
