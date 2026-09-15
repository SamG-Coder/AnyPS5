#include "IntermediateRepresentation/IrOpcode.hpp"
#include <stdexcept>

namespace ShaderRecompiler {

std::size_t IrOpcodeOperandCount(IrOpcode opcode) {
    throw std::runtime_error("IrOpcodeOperandCount not implemented");
}

IrType IrOpcodeType(IrOpcode opcode) {
    throw std::runtime_error("IrOpcodeType not implemented");
}

IrType IrOpcodeArgumentType(IrOpcode opcode, std::size_t index) {
    throw std::runtime_error("IrOpcodeArgumentType not implemented");
}

bool IrOpcodeHasSideEffects(IrOpcode opcode) {
    throw std::runtime_error("IrOpcodeHasSideEffects not implemented");
}

BufferAccess BufferAccessOf(IrOpcode opcode) {
    throw std::runtime_error("BufferAccessOf not implemented");
}

std::uint32_t BufferComponentCount(IrOpcode opcode) {
    throw std::runtime_error("BufferComponentCount not implemented");
}

SharedAccess SharedAccessOf(IrOpcode opcode) {
    throw std::runtime_error("SharedAccessOf not implemented");
}

std::uint32_t SharedComponentCount(IrOpcode opcode) {
    throw std::runtime_error("SharedComponentCount not implemented");
}

AddressOpcodeInfo AddressOpcodeInfoOf(IrOpcode opcode) {
    throw std::runtime_error("AddressOpcodeInfoOf not implemented");
}

ImageOpcodeInfo ImageOpcodeInfoOf(IrOpcode opcode) {
    throw std::runtime_error("ImageOpcodeInfoOf not implemented");
}

std::string_view IrOpcodeName(IrOpcode opcode) {
    throw std::runtime_error("IrOpcodeName not implemented");
}

}
