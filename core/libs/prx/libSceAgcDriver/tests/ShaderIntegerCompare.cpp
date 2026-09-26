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

std::uint32_t evaluate(const IrValue& value, const std::array<std::uint32_t, 2>& inputs, bool active) {
    if (value.HasImmediate()) return value.ImmediateU32();
    const auto arg = [&](std::size_t index) { return evaluate(*value.Argument(index), inputs, active); };
    switch (value.Opcode()) {
        case IrOpcode::GetExec: return active;
        case IrOpcode::GetVectorRegister: return inputs.at(value.Argument(0)->Register().index);
        case IrOpcode::Identity: return arg(0);
        case IrOpcode::BitFieldUExtract: return (arg(0) >> arg(1)) & ((1u << arg(2)) - 1u);
        case IrOpcode::BitFieldSExtract: {
            const auto shifted = arg(0) << (32u - arg(1) - arg(2));
            return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(shifted) >> (32u - arg(2)));
        }
        case IrOpcode::IEqual32: return arg(0) == arg(1);
        case IrOpcode::INotEqual32: return arg(0) != arg(1);
        case IrOpcode::SLessThan32: return std::bit_cast<std::int32_t>(arg(0)) < std::bit_cast<std::int32_t>(arg(1));
        case IrOpcode::SLessThanEqual32: return std::bit_cast<std::int32_t>(arg(0)) <= std::bit_cast<std::int32_t>(arg(1));
        case IrOpcode::SGreaterThan32: return std::bit_cast<std::int32_t>(arg(0)) > std::bit_cast<std::int32_t>(arg(1));
        case IrOpcode::SGreaterThanEqual32: return std::bit_cast<std::int32_t>(arg(0)) >= std::bit_cast<std::int32_t>(arg(1));
        case IrOpcode::LogicalAnd: return arg(0) && arg(1);
        default: throw std::runtime_error("unexpected IR in integer comparison");
    }
}

}

int main() {
    try {
        const std::array expectedOps{RdnaOpcode::VCmpxLtI16, RdnaOpcode::VCmpxEqI16, RdnaOpcode::VCmpxLeI16,
                                    RdnaOpcode::VCmpxGtI16, RdnaOpcode::VCmpxNeI16, RdnaOpcode::VCmpxGeI16};
        for (std::uint32_t operation = 0; operation < expectedOps.size(); ++operation) {
            const auto opcode = 0x99u + operation;
            const std::array code{0x7c000300u | (opcode << 17u), 0xbf810000u};
            const auto instruction = RdnaInstructionDecoder{}.Decode(code).instructions.at(0);
            require(instruction.op == expectedOps[operation] && instruction.destination.kind == RdnaOperandKind::ExecLo,
                    "incorrect signed short comparison decoding");
            const std::array extended{0xd400007eu | (opcode << 16u), 0x00020300u, 0xbf810000u};
            require(RdnaInstructionDecoder{}.Decode(extended).instructions.at(0).op == instruction.op,
                    "VOP3 short comparison differs from VOPC");
            for (const auto modifier : {0x100u, 0x800u}) {
                bool rejected = false;
                try {
                    const std::array invalid{extended[0] | modifier, extended[1], extended[2]};
                    (void)RdnaInstructionDecoder{}.Decode(invalid);
                } catch (const std::invalid_argument&) { rejected = true; }
                require(rejected, "unsupported short comparison modifier was ignored");
            }
            IrProgram program;
            auto& block = program.CreateBlock();
            TranslationContext context(program, block, 256);
            context.TranslateInstruction(instruction);
            const IrValue* result = nullptr;
            for (const auto* value : block.Instructions()) {
                if (value->Opcode() == IrOpcode::SetExec) result = value->Argument(0);
                require(value->Opcode() != IrOpcode::SetVccLo && value->Opcode() != IrOpcode::SetVccHi,
                        "CMPX modified VCC");
            }
            require(result != nullptr, "short comparison did not update EXEC");
            for (const auto lhs : {-32768, -32767, -1, 0, 1, 32766, 32767}) {
                for (const auto rhs : {-32768, -32767, -1, 0, 1, 32766, 32767}) {
                    const std::array expected{lhs < rhs, lhs == rhs, lhs <= rhs, lhs > rhs, lhs != rhs, lhs >= rhs};
                    const std::array inputs{(static_cast<std::uint32_t>(lhs) & 0xffffu) | 0xabcd0000u,
                                            (static_cast<std::uint32_t>(rhs) & 0xffffu) | 0x12340000u};
                    require(evaluate(*result, inputs, true) == expected[operation], "comparison lost signed short semantics");
                    require(evaluate(*result, inputs, false) == 0, "comparison reactivated an inactive lane");
                }
            }
        }
        std::puts("Integer comparison shader tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
