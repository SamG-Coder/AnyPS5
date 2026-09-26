#include "ControlFlow/GraphBuilder.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/InstructionTranslator.hpp"
#include "Optimization/SsaBuilder.hpp"
#include "Optimization/ConstantFolder.hpp"
#include <array>
#include <stdexcept>

int main() {
    using namespace ShaderRecompiler;
    const std::array code{0xbf810000u};
    const auto decoded = RdnaInstructionDecoder{}.Decode(code);
    const auto cfg = GraphBuilder{}.Build(decoded);
    ShaderVertexInputInfo vertex{};
    for (const auto wave : {32u, 64u}) {
        for (const auto base : {0u, 8u}) {
            TranslateOptions options{};
            options.stage = ShaderStageKind::Vertex;
            options.waveSize = wave;
            options.userDataBaseRegister = base;
            options.userDataCount = 4;
            options.inputInfo.vertex = &vertex;
            auto program = InstructionTranslator{}.Translate(decoded, cfg, options);
            bool found = false;
            for (const auto& block : program.Blocks()) {
                for (const auto* value : block->Instructions()) {
                    if (value->Opcode() != IrOpcode::SetScalarRegister || value->Argument(0)->Register().index != 3) continue;
                    if (found) throw std::runtime_error("vertex entry overwrites s3");
                    found = true;
                    const auto* input = value->Argument(1);
                    if (base == 0) {
                        if (input->Opcode() != IrOpcode::GetUserData) throw std::runtime_error("vertex entry overwrites user data");
                    } else {
                        const auto merged = input->ImmediateU32();
                        if ((merged & 0xffu) != wave || (merged & 0xffffff00u) != 0)
                            throw std::runtime_error("invalid passthrough vertex wave state");
                    }
                }
            }
            if (!found) throw std::runtime_error("missing vertex s3 entry state");
            if (base == 8u) {
                const std::array gate{0x99030380u, 0x94fe03c1u, 0xbf810000u};
                const auto gateCode = RdnaInstructionDecoder{}.Decode(gate);
                auto gated = InstructionTranslator{}.Translate(gateCode, GraphBuilder{}.Build(gateCode), options);
                IrBuilder observe(gated);
                observe.SetInsertionPoint(*gated.BlockOrder().back());
                auto& low = observe.GetExecLo();
                auto& high = observe.GetExecHi();
                SsaBuilder{}.Rewrite(gated);
                ConstantFolder{}.Fold(gated);
                if (low.Resolve()->ImmediateU32() != 0xffffffffu ||
                    high.Resolve()->ImmediateU32() != (wave == 64u ? 0xffffffffu : 0u))
                    throw std::runtime_error("vertex lane gate disables valid invocations");
            }
        }
    }
}
