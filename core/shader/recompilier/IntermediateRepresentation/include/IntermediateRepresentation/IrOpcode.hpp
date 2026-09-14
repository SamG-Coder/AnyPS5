#ifndef SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IROPCODE_HPP
#define SHADER_RECOMPILIER_INTERMEDIATEREPRESENTATION_IROPCODE_HPP

#include <cstdint>

namespace ShaderRecompiler {

enum class IrOpcode : std::uint16_t {
    Identity,
    Phi,
    GetRegister,
    SetRegister,
    IAdd32,
    ISub32,
    IMul32,
    IDiv32,
    IMod32,
    IAnd32,
    IOr32,
    IXor32,
    IShiftLeft32,
    IShiftRightLogical32,
    IShiftRightArithmetic32,
    INegate32,
    INot32,
    FAdd32,
    FSub32,
    FMul32,
    FDiv32,
    FNegate32,
    FAbs32,
    FFma32,
    FSqrt32,
    FRcp32,
    FRsq32,
    FMin32,
    FMax32,
    FFloor32,
    FCeil32,
    FFract32,
    ConvertFToI32,
    ConvertIToF32,
    ConvertFToU32,
    ConvertUToF32,
    CompareEqual,
    CompareNotEqual,
    CompareLessThan,
    CompareGreaterThan,
    Select,
    Branch,
    BranchConditional,
    Loop,
    LoopMerge,
    Return,
    Unreachable,
    LoadBuffer,
    StoreBuffer,
    LoadImage,
    StoreImage,
    ImageSampleImplicitLod,
    ImageSampleExplicitLod,
    LoadShared,
    StoreShared,
    Barrier,
    EmitVertex,
    Discard,
    ReadFirstLane,
    ReadLane,
    Ballot
};

[[nodiscard]] std::uint32_t IrOpcodeOperandCount(IrOpcode opcode);

}

#endif
