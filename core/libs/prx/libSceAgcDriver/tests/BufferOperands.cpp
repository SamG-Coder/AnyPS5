#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/TranslationContext.hpp"
#include <array>
#include <stdexcept>

void CheckOperand(const ShaderRecompiler::IrValue* value, bool enabled, ShaderRecompiler::IrOpcode opcode, unsigned reg) {
    if (!enabled) {
        if (!value->HasImmediate() || value->ImmediateU32() != 0) throw std::runtime_error("disabled address component is not zero");
    } else if (value->Opcode() != opcode || value->Argument(0)->Register().index != reg) {
        throw std::runtime_error("buffer address reads the wrong register");
    }
}

int main() {
    using namespace ShaderRecompiler;
    for (const auto typed : {false, true}) {
        for (const auto operation : {0u, 1u, 2u}) {
            if (typed && operation == 2) continue;
            for (const auto flags : {0u, 0x1000u, 0x2000u, 0x3000u}) {
                for (const auto scalar : {0x80u, 12u}) {
                    const auto opcode = operation == 0 ? 0x0cu : operation == 1 ? 0x1cu : 0x32u;
                    const auto word0 = typed ? 0xe8000000u | ((operation == 1 ? 4u : 0u) << 16u) | (4u << 19u) | (7u << 23u) : 0xe0000000u | (opcode << 18u);
                    const std::array code{word0 | flags | 20u, (scalar << 24u) | (2u << 16u) | (20u << 8u) | 5u, 0xbf810000u};
                    const auto decoded = RdnaInstructionDecoder{}.Decode(code);
                    IrProgram program;
                    auto& block = program.CreateBlock();
                    TranslationContext context(program, block, 256);
                    context.TranslateInstruction(decoded.instructions.front());
                    const auto expected = operation == 0 ? IrOpcode::LoadBufferU32 : operation == 1 ? IrOpcode::StoreBufferU32 : IrOpcode::BufferAtomicIAdd32;
                    const IrValue* memory = nullptr;
                    for (const auto* value : block.Instructions()) if (value->Opcode() == expected) memory = value;
                    if (!memory) throw std::runtime_error("missing translated buffer instruction");
                    CheckOperand(memory->Argument(1), (flags & 0x2000u) != 0, IrOpcode::GetVectorRegister, 5);
                    CheckOperand(memory->Argument(2), (flags & 0x1000u) != 0, IrOpcode::GetVectorRegister, (flags & 0x2000u) != 0 ? 6 : 5);
                    CheckOperand(memory->Argument(3), scalar != 0x80u, IrOpcode::GetScalarRegister, scalar);
                    if (program.Resources().memoryInfo.front().offset != 20u) throw std::runtime_error("lost immediate buffer offset");
                }
            }
        }
    }
}

