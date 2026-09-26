#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/TranslationContext.hpp"
#include <array>
#include <cstdio>
#include <stdexcept>

namespace {

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

}

int main() {
    using namespace ShaderRecompiler;
    try {
        for (const std::uint32_t count : {0u, 1u, 63u}) {
            const std::array code{0xbfa10000u | count, 0xbe800380u, 0xbf810000u};
            const auto decoded = RdnaInstructionDecoder{}.Decode(code);
            require(decoded.instructions.size() == 3, "clause marker skipped following instructions");
            const auto& clause = decoded.instructions[0];
            require(clause.op == RdnaOpcode::SClause && clause.opcodeId == 0x21u, "incorrect clause opcode");
            require(clause.sourceCount == 1 && clause.source0.value == count, "clause immediate was lost");
            require(decoded.instructions[1].programCounter == 4 && decoded.instructions[1].op == RdnaOpcode::SMovB32,
                "clause marker changed the next instruction");
            require(decoded.instructions[2].programCounter == 8 && decoded.instructions[2].op == RdnaOpcode::SEndpgm,
                "clause marker changed program termination");
            IrProgram program;
            auto& block = program.CreateBlock();
            TranslationContext context(program, block, 256);
            context.TranslateInstruction(clause);
            require(block.Instructions().size() == 1 && block.Instructions().front()->Opcode() == IrOpcode::ControlNop,
                "clause scheduling marker changed shader state");
            context.TranslateInstruction(decoded.instructions[1]);
            require(block.Instructions().size() > 1, "instruction after clause was not translated");
        }
        bool rejected = false;
        try {
            const std::array code{0xbfff0000u};
            (void)RdnaInstructionDecoder{}.Decode(code);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        require(rejected, "unknown SOPP opcode was silently accepted");
        std::puts("Shader clause tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
