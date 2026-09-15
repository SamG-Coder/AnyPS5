#include "Recompiler.hpp"
#include "ControlFlow/include/ControlFlow/GraphBuilder.hpp"
#include "ControlFlow/include/ControlFlow/Structurizer.hpp"
#include "RdnaDecoder/include/RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "Optimization/include/Optimization/BindingAllocator.hpp"
#include "Optimization/include/Optimization/ConstantFolder.hpp"
#include "Optimization/include/Optimization/DeadCodeEliminator.hpp"
#include "Optimization/include/Optimization/ReadLaneEliminator.hpp"
#include "Optimization/include/Optimization/ResourceTracker.hpp"
#include "Optimization/include/Optimization/ShaderInfoCollector.hpp"
#include "Optimization/include/Optimization/SrtWalker.hpp"
#include "Optimization/include/Optimization/SsaBuilder.hpp"
#include "SpirvBackend/include/SpirvBackend/SpirvEmitter.hpp"
#include "Translation/include/Translation/InstructionTranslator.hpp"
#include <exception>
#include <stdexcept>
#include <string>

namespace ShaderRecompiler {

namespace {

RecompileResult RecompileImpl(const RecompileRequest& request) {
    const RdnaInstructionDecoder decoder;
    const auto decoded = decoder.Decode(request.shader.code);

    const GraphBuilder graphBuilder;
    auto cfg = graphBuilder.Build(decoded);

    const Structurizer structurizer;
    structurizer.Structurize(cfg);

    TranslateOptions translateOptions {};
    translateOptions.waveSize = request.context.waveSize;
    translateOptions.userDataBaseRegister = request.context.userDataBaseRegister;
    translateOptions.userDataCount = static_cast<std::uint32_t>(request.context.userData.size());
    translateOptions.embeddedFetch = nullptr;

    const InstructionTranslator translator;
    auto program = translator.Translate(decoded, cfg, translateOptions);

    const SsaBuilder ssaBuilder;
    ssaBuilder.Rewrite(program);

    const ConstantFolder constantFolder;
    constantFolder.Fold(program);

    const DeadCodeEliminator deadCodeEliminator;
    deadCodeEliminator.Eliminate(program);

    const ReadLaneEliminator readLaneEliminator;
    readLaneEliminator.Eliminate(program, translateOptions.waveSize);

    const SrtWalker srtWalker;
    srtWalker.BuildPlan(program);

    const ResourceTracker resourceTracker;
    resourceTracker.Track(program);

    const ShaderInfoCollector shaderInfoCollector;
    shaderInfoCollector.Collect(program);

    const BindingAllocator bindingAllocator;
    const auto bindings = bindingAllocator.Allocate(program, request.layout.pushConstantOffsetBytes);

    SpirvTargetOptions targetOptions {};
    targetOptions.vulkanVersion = request.target.vulkanVersion;
    targetOptions.spirvVersion = request.target.spirvVersion;
    targetOptions.subgroupSize = request.target.subgroupSize;

    const SpirvEmitter spirvEmitter;
    const auto spirv = spirvEmitter.Emit(program, bindings, targetOptions);

    RecompileResult result;
    result.spirv = spirv;
    result.bindings = bindings.bindings;
    return result;
}

}

RecompileResult Recompile(const RecompileRequest& request) {
    try {
        return RecompileImpl(request);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("ShaderRecompiler::Recompile: ") + e.what());
    } catch (...) {
        throw std::runtime_error("ShaderRecompiler::Recompile: unknown exception");
    }
}

}
