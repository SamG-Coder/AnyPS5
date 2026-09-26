#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/TranslationContext.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace {

using namespace ShaderRecompiler;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::uint32_t evaluate(const IrValue& value, std::array<std::uint32_t, 2> inputs) {
    if (value.HasImmediate()) {
        return value.Type() == IrType::F32 ? std::bit_cast<std::uint32_t>(value.ImmediateF32()) : value.ImmediateU32();
    }
    const auto arg = [&](std::size_t index) { return evaluate(*value.Argument(index), inputs); };
    switch (value.Opcode()) {
        case IrOpcode::GetExec: return 1u;
        case IrOpcode::GetVectorRegister: return inputs.at(value.Argument(0)->Register().index);
        case IrOpcode::Identity:
        case IrOpcode::BitCastU32F32:
        case IrOpcode::BitCastF32U32: return arg(0);
        case IrOpcode::BitwiseAnd32: return arg(0) & arg(1);
        case IrOpcode::IEqual32: return arg(0) == arg(1);
        case IrOpcode::LogicalOr: return arg(0) || arg(1);
        case IrOpcode::SelectU32:
        case IrOpcode::SelectF32: return arg(0) ? arg(1) : arg(2);
        case IrOpcode::FPMul32: return std::bit_cast<std::uint32_t>(std::bit_cast<float>(arg(0)) * std::bit_cast<float>(arg(1)));
        default: throw std::runtime_error("unexpected IR opcode in multiply expression");
    }
}

}

int main() {
    try {
        const std::array code{0x0e040300u, 0xbf810000u};
        const auto decoded = RdnaInstructionDecoder{}.Decode(code);
        const auto& multiply = decoded.instructions.at(0);
        require(multiply.op == RdnaOpcode::VMulLegacyF32 && multiply.sourceCount == 2, "incorrect legacy multiply decoding");
        require(multiply.destination.reg == 2 && multiply.source0.reg == 0 && multiply.source1.reg == 1,
            "incorrect legacy multiply operands");
        IrProgram program;
        auto& block = program.CreateBlock();
        TranslationContext context(program, block, 256);
        context.TranslateInstruction(multiply);
        const IrValue* result = nullptr;
        for (const auto* value : block.Instructions()) {
            if (value->Opcode() == IrOpcode::SetVectorRegister) result = value->Argument(1);
        }
        require(result != nullptr, "legacy multiply did not write its destination");
        for (const auto zero : {0u, 0x80000000u}) {
            for (const auto other : {0u, 0x80000000u, 0x3f800000u, 0xbf800000u, 0x7f800000u, 0xff800000u, 0x7fc00000u, 0x7f800001u}) {
                require(evaluate(*result, {zero, other}) == 0u, "zero times operand did not produce positive zero");
                require(evaluate(*result, {other, zero}) == 0u, "operand times zero did not produce positive zero");
            }
        }
        require(evaluate(*result, {0xc0000000u, 0x40400000u}) == 0xc0c00000u, "finite product is incorrect");
        require(evaluate(*result, {0x7f800000u, 0xbf800000u}) == 0xff800000u, "infinite product is incorrect");
        require(std::isnan(std::bit_cast<float>(evaluate(*result, {0x7fc00000u, 0x3f800000u}))), "nonzero multiplication lost NaN");
        const std::array normalCode{0x10040300u, 0xbf810000u};
        require(RdnaInstructionDecoder{}.Decode(normalCode).instructions[0].op == RdnaOpcode::VMulF32,
            "ordinary multiply was changed");
        try {
            const std::array unknown{0u, 0xbf810000u};
            (void)RdnaInstructionDecoder{}.Decode(unknown);
            throw std::runtime_error("unknown vector opcode was accepted");
        } catch (const std::invalid_argument& error) {
            require(std::string(error.what()) == "VOP2 opcode is not implemented: 0", "missing numeric opcode diagnostic");
        }
        std::puts("Legacy shader multiply tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
