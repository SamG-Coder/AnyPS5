#ifndef SHADER_RECOMPILIER_RDNADECODER_RDNAINSTRUCTION_HPP
#define SHADER_RECOMPILIER_RDNADECODER_RDNAINSTRUCTION_HPP

#include <RdnaDecoder/RdnaOpcode.hpp>
#include <cstdint>

namespace ShaderRecompiler {

enum class RdnaOperandKind {
    None,
    ScalarRegister,
    VectorRegister,
    VccLo,
    VccHi,
    ExecLo,
    ExecHi,
    Scc,
    Null,
    LiteralConstant,
    IntegerInlineConstant,
    FloatInlineConstant
};

struct RdnaOperand {
    RdnaOperandKind kind;
    std::uint32_t reg;
    std::uint32_t value;
    bool negate;
    bool absolute;
};

struct RdnaInstruction {
    RdnaOpcode op;
    std::uint32_t programCounter;
    RdnaOperand destination;
    RdnaOperand source0;
    RdnaOperand source1;
    RdnaOperand source2;
    RdnaOperand source3;
    std::int32_t branchOffset;
    std::uint32_t memoryOffset;
    std::uint32_t dataDwordCount;
    bool clampResult;
    bool is64Bit;
};

}

#endif
