#ifndef SHADER_RECOMPILIER_OPTIMIZATION_RESOURCEMATERIALIZER_HPP
#define SHADER_RECOMPILIER_OPTIMIZATION_RESOURCEMATERIALIZER_HPP

#include <IntermediateRepresentation/IrProgram.hpp>
#include <cstdint>
#include <vector>

namespace ShaderRecompiler {

struct ResourceSpecialization {
    std::vector<std::uint32_t> boundDescriptors;
};

class ResourceMaterializer {
public:
    void Apply(IrProgram& program, const ResourceSpecialization& specialization) const;
};

}

#endif
