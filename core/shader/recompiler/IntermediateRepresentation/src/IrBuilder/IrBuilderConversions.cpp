#include "IntermediateRepresentation/IrBuilder.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

IrValue& IrBuilder::BitCastF32(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastF32 not implemented");
}

IrValue& IrBuilder::BitCastU32(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastU32 not implemented");
}

IrValue& IrBuilder::BitCastF16(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastF16 not implemented");
}

IrValue& IrBuilder::BitCastU32FromF16(IrValue& value) {
    throw std::runtime_error("IrBuilder::BitCastU32FromF16 not implemented");
}

IrValue& IrBuilder::ConstructU64(IrValue& low, IrValue& high) {
    throw std::runtime_error("IrBuilder::ConstructU64 not implemented");
}

IrValue& IrBuilder::CompositeExtract(IrValue& composite, std::uint32_t index) {
    throw std::runtime_error("IrBuilder::CompositeExtract not implemented");
}

}
