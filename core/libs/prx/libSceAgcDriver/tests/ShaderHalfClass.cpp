#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/TranslationContext.hpp"
#include <array>
#include <bit>
#include <cstdio>
#include <stdexcept>

namespace {

using namespace ShaderRecompiler;

void require(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(reason);
}

std::uint32_t evaluate(const IrValue& value, std::array<std::uint32_t, 2> inputs, bool active) {
    if (value.HasImmediate()) return value.Type() == IrType::F32 ? std::bit_cast<std::uint32_t>(value.ImmediateF32()) : value.ImmediateU32();
    const auto arg = [&](std::size_t index) { return evaluate(*value.Argument(index), inputs, active); };
    switch (value.Opcode()) {
        case IrOpcode::GetExec: return active;
        case IrOpcode::GetVectorRegister: return inputs.at(value.Argument(0)->Register().index);
        case IrOpcode::Identity: return arg(0);
        case IrOpcode::BitCastU32F32:
        case IrOpcode::BitCastF32U32: return arg(0);
        case IrOpcode::FPCmpClass32: {
            const auto bits = arg(0);
            require((bits & 0x7f800000u) != 0 && (bits & 0x7f800000u) != 0x7f800000u,
                    "inline float constant is not normal");
            return (arg(1) & ((bits & 0x80000000u) != 0 ? 8u : 256u)) != 0;
        }
        case IrOpcode::BitFieldUExtract: return (arg(0) >> arg(1)) & ((1u << arg(2)) - 1u);
        case IrOpcode::BitwiseAnd32: return arg(0) & arg(1);
        case IrOpcode::BitwiseXor32: return arg(0) ^ arg(1);
        case IrOpcode::IEqual32: return arg(0) == arg(1);
        case IrOpcode::INotEqual32: return arg(0) != arg(1);
        case IrOpcode::LogicalAnd: return arg(0) && arg(1);
        case IrOpcode::SelectU32: return arg(0) ? arg(1) : arg(2);
        default: throw std::runtime_error("unexpected IR in half-class expression");
    }
}

std::uint32_t referenceClass(std::uint32_t bits) {
    const bool negative = (bits & 0x8000u) != 0;
    const auto magnitude = bits & 0x7fffu;
    if (magnitude > 0x7c00u) return (bits & 0x200u) != 0 ? 2u : 1u;
    if (magnitude == 0x7c00u) return negative ? 4u : 512u;
    if (magnitude == 0) return negative ? 32u : 64u;
    if (magnitude < 0x400u) return negative ? 16u : 128u;
    return negative ? 8u : 256u;
}

void testClassification(RdnaInstruction instruction, bool absolute, bool negate) {
    instruction.source0.absolute = absolute;
    instruction.source0.negate = negate;
    IrProgram program;
    auto& block = program.CreateBlock();
    TranslationContext context(program, block, 256);
    context.TranslateInstruction(instruction);
    const IrValue* result = nullptr;
    bool low = false;
    bool high = false;
    for (const auto* value : block.Instructions()) {
        if (value->Opcode() == IrOpcode::SetExec) result = value->Argument(0);
        if (value->Opcode() == IrOpcode::SetExecLo) low = true;
        if (value->Opcode() == IrOpcode::SetExecHi) high = true;
        require(value->Opcode() != IrOpcode::SetVccLo && value->Opcode() != IrOpcode::SetVccHi,
                "CMPX modified VCC");
    }
    require(result != nullptr && low && high, "CMPX did not update the complete execution mask");
    for (std::uint32_t bits = 0; bits < 0x10000u; ++bits) {
        auto modified = absolute ? bits & 0x7fffu : bits;
        if (negate) modified ^= 0x8000u;
        const auto expected = referenceClass(modified);
        const auto input = bits | 0xa5a50000u;
        require(evaluate(*result, {input, expected}, true) == 1, "half value did not match its class");
        require(evaluate(*result, {input, ~expected}, true) == 0, "half value matched a different class");
        require(evaluate(*result, {input, 0x3ffu}, false) == 0, "CMPX reactivated an inactive lane");
    }
}

}

int main() {
    try {
        const std::array code{0x7d3e0300u, 0xbf810000u};
        const auto decoded = RdnaInstructionDecoder{}.Decode(code);
        const auto instruction = decoded.instructions.at(0);
        require(instruction.op == RdnaOpcode::VCmpxClassF16 && instruction.destination.kind == RdnaOperandKind::ExecLo,
                "incorrect half-class opcode or destination");
        require(instruction.sourceCount == 2 && instruction.source0.reg == 0 && instruction.source1.reg == 1,
                "incorrect half-class operands");
        for (const bool absolute : {false, true})
            for (const bool negate : {false, true}) testClassification(instruction, absolute, negate);
        const std::array extended{0xd49f007eu, 0x00020300u, 0xbf810000u};
        const auto wide = RdnaInstructionDecoder{}.Decode(extended).instructions.at(0);
        require(wide.op == instruction.op && wide.destination.kind == RdnaOperandKind::ExecLo,
                "VOP3 half-class alias did not target EXEC");
        for (std::uint32_t source = 240; source <= 248; ++source) {
            const std::array inlineCode{0x7d3e0200u | source, 0xbf810000u};
            const auto inlineInstruction = RdnaInstructionDecoder{}.Decode(inlineCode).instructions.at(0);
            IrProgram program;
            auto& block = program.CreateBlock();
            TranslationContext context(program, block, 256);
            context.TranslateInstruction(inlineInstruction);
            bool checked = false;
            for (const auto* value : block.Instructions()) {
                if (value->Opcode() != IrOpcode::SetExec) continue;
                const auto normalClass = source < 248 && (source & 1u) != 0 ? 8u : 256u;
                require(evaluate(*value->Argument(0), {0, normalClass}, true) == 1,
                        "inline float was reinterpreted as half zero");
                checked = true;
            }
            require(checked, "inline half-class instruction lost EXEC result");
        }
        std::puts("Half-class shader tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
