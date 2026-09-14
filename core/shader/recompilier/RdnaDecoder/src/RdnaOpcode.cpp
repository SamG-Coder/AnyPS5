#include <RdnaDecoder/RdnaOpcode.hpp>
#include <stdexcept>

namespace ShaderRecompiler {

bool IsScalarAluOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsScalarAluOpcode not implemented");
}
bool IsVectorAluOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsVectorAluOpcode not implemented");
}
bool IsBranchOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsBranchOpcode not implemented");
}
bool IsScalarMemoryOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsScalarMemoryOpcode not implemented");
}
bool IsBufferMemoryOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsBufferMemoryOpcode not implemented");
}
bool IsImageOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsImageOpcode not implemented");
}
bool IsExportOpcode(RdnaOpcode opcode) {
    throw std::runtime_error("IsExportOpcode not implemented");
}

}
