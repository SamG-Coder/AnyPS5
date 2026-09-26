#include "prx/libSceAgcDriver/Graphics/include/ShaderInputState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Context.hpp"
#include "ControlFlow/GraphBuilder.hpp"
#include "ControlFlow/RequestSerializer.hpp"
#include "CacheKey.hpp"
#include "Optimization/DeadCodeEliminator.hpp"
#include "Optimization/ShaderInfoCollector.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Translation/ShaderInputInfoBuilder.hpp"
#include "Translation/InstructionTranslator.hpp"
#include <array>
#include <bit>
#include <string_view>

void RunPixelInputTests() {
    using namespace ShaderRecompiler;
    using AgcDriver::Graphics::Require;
    AgcDriver::Registers registers{{0x1b6, 0}, {0x1b3, 0x80}, {0x1b4, 0x80}, {0x203, 0}, {0x1c5, 0}};
    for (const auto mask : {0u, 0x80u, 0x180u, 0x182u, 0x1b3u}) {
        registers[0x1b3] = registers[0x1b4] = mask;
        GuestContext guest{};
        guest.pixel = AgcDriver::Graphics::DecodePixelStageInfo(registers, false, 0);
        const auto& pixel = *guest.pixel;
        RecompileRequest request{};
        request.context = guest;
        const auto restored = RequestSerializer{}.Deserialize(RequestSerializer{}.Serialize(request));
        Require(restored.request.context.pixel->systemInputBase == pixel.systemInputBase &&
            restored.request.context.pixel->lineStipple == pixel.lineStipple, "pixel layout serialization lost fields");
        std::vector<std::uint64_t> originalKey, changedKey;
        RecompileCacheKey::Build(request, originalKey);
        request.context.pixel->lineStipple = !pixel.lineStipple;
        RecompileCacheKey::Build(request, changedKey);
        Require(originalKey != changedKey, "shader cache ignored line-stipple input");
        request.context = guest;
        ++request.context.pixel->systemInputBase;
        RecompileCacheKey::Build(request, changedKey);
        Require(originalKey != changedKey, "shader cache ignored system input register base");
        const auto base = 2u * static_cast<std::uint32_t>(std::popcount(mask & 0x33u)) + ((mask & 0x80u) != 0);
        Require(pixel.systemInputBase == base && pixel.lineStipple == ((mask & 0x80u) != 0), "pixel input slots decoded incorrectly");
        auto inputs = BuildShaderStageInputInfo(ShaderStageKind::Pixel, guest);
        Require(inputs.pixel->psSystemInputBase == base, "pixel system register base was lost");
        const std::array code{0xbf810000u};
        const auto decoded = RdnaInstructionDecoder{}.Decode(code);
        const auto cfg = GraphBuilder{}.Build(decoded);
        TranslateOptions options{};
        options.stage = ShaderStageKind::Pixel;
        options.userDataCount = 0;
        options.inputInfo = inputs;
        auto translated = InstructionTranslator{}.Translate(decoded, cfg, options);
        bool position = false;
        bool stipple = false;
        for (const auto& block : translated.Blocks()) {
            for (const auto* value : block->Instructions()) {
                if (value->Opcode() != IrOpcode::SetVectorRegister) continue;
                const auto* source = value->Argument(1);
                if (source->Opcode() != IrOpcode::GetBuiltin) continue;
                const auto kind = static_cast<StageInputKind>(source->Argument(0)->ImmediateU32());
                const auto reg = value->Argument(0)->Register().index;
                if (kind == StageInputKind::FragCoord) position = reg == base;
                if (kind == StageInputKind::LineStipple) stipple = reg + 1u == base;
            }
        }
        Require(position == ((mask & 0x100u) != 0) && stipple == ((mask & 0x80u) != 0), "pixel prologue assigned the wrong registers");
        IrProgram program;
        program.Resources().stage = IrShaderStage::Pixel;
        program.Resources().resourceTrackingComplete = true;
        auto& block = program.CreateBlock();
        program.SetEntryBlock(block);
        program.BlockOrder().push_back(&block);
        IrBuilder builder(program);
        builder.SetInsertionPoint(block);
        (void)builder.Emit(IrOpcode::GetBuiltin, IrType::U32,
            {&builder.Constant(static_cast<std::uint32_t>(StageInputKind::LineStipple)), &builder.Constant(0u)});
        bool rejected = false;
        try { ShaderInfoCollector{}.Collect(program, inputs); }
        catch (const std::runtime_error& error) {
            if (std::string_view(error.what()).find("line-stipple input") == std::string_view::npos) throw;
            rejected = true;
        }
        Require(rejected, "live line-stipple input was silently accepted");
        DeadCodeEliminator{}.Eliminate(program);
        ShaderInfoCollector{}.Collect(program, inputs);
    }
}
