#include "Recompiler.hpp"
#include "ControlFlow/RequestSerializer.hpp"
#include "RdnaDecoder/RdnaInstructionDecoder.hpp"
#include "ControlFlow/GraphBuilder.hpp"
#include "ControlFlow/Structurizer.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"
#include "Optimization/BindingAllocator.hpp"
#include "Optimization/ConstantFolder.hpp"
#include "Optimization/DeadCodeEliminator.hpp"
#include "Optimization/ReadLaneEliminator.hpp"
#include "Optimization/ResourceTracker.hpp"
#include "Optimization/ShaderInfoCollector.hpp"
#include "Optimization/SrtWalker.hpp"
#include "Optimization/SsaBuilder.hpp"
#include "Translation/InstructionTranslator.hpp"
#include "Translation/ShaderInputInfoBuilder.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace ShaderRecompiler;

static std::string readFile(const char* path) {
    std::ifstream f(path, std::ios::binary);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: Deserialize <payload.b64>\n";
        return 1;
    }
    std::string b64 = readFile(argv[1]);
    while (!b64.empty() && (b64.back() == '\n' || b64.back() == '\r' || b64.back() == ' ')) b64.pop_back();

    RequestSerializer serializer;
    DeserializedRequest req;
    try {
        req = serializer.Deserialize(b64);
    } catch (const std::exception& e) {
        std::cerr << "Deserialize failed: " << e.what() << "\n";
        return 1;
    }

    std::cout << "shader code words: " << req.request.shader.code.size() << "\n";
    std::cout << "shader stage: " << static_cast<int>(req.request.shader.stage) << "\n";
    std::cout << "codeAddress: 0x" << std::hex << req.request.shader.codeAddress << std::dec << "\n";
    std::cout << "header bytes: " << req.request.shader.header.size() << "\n";
    std::cout << "userData count: " << req.request.context.userData.size() << "\n";
    std::cout << "waveSize: " << req.request.context.waveSize << "\n";
    std::cout << "userDataBaseRegister: " << req.request.context.userDataBaseRegister << "\n";
    std::cout << "memory regions: " << req.request.context.memory.size() << "\n";
    for (auto& m : req.request.context.memory) {
        std::cout << "  region addr=0x" << std::hex << m.guestAddress << std::dec << " size=" << m.bytes.size() << "\n";
    }
    if (req.request.context.vertex) {
        auto& v = *req.request.context.vertex;
        std::cout << "vertex resourcesNum=" << v.resourcesNum << " fetchAttribReg=" << v.fetchAttribReg
                  << " fetchBufferReg=" << v.fetchBufferReg << " fetchEmbedded=" << v.fetchEmbedded << "\n";
        for (std::uint32_t i = 0; i < v.resourcesNum; i++) {
            auto& d = v.resourcesDst[i];
            std::cout << "  resource[" << i << "] fields=";
            for (auto f : v.resources[i].fields) std::cout << std::hex << f << " ";
            std::cout << std::dec << " dst registerStart=" << d.registerStart << " registersNum=" << d.registersNum
                      << " attrId=" << d.attrId << " fetchIndex=" << d.fetchIndex << "\n";
        }
    }
    for (std::uint32_t i = 0; i < req.request.context.userData.size(); i++) {
        std::cout << "userData[" << i << "]=0x" << std::hex << req.request.context.userData[i] << std::dec << "\n";
    }
    if (req.request.context.compute) {
        auto& c = *req.request.context.compute;
        std::cout << "compute numThreads=" << c.numThreads[0] << "," << c.numThreads[1] << "," << c.numThreads[2]
                  << " ldsSizeDwords=" << c.ldsSizeDwords << "\n";
    }

    RdnaInstructionDecoder decoder;
    auto decoded = decoder.Decode(req.request.shader.code);
    std::string disasm = RdnaProgramToString(decoded);
    {
        std::ofstream out("disasm.txt");
        out << disasm;
    }
    std::cout << "Disassembly written to disasm.txt (" << disasm.size() << " bytes)\n";

    try {
        const auto stageKind = static_cast<ShaderStageKind>(0);
    } catch (...) {}

    try {
        ShaderStageKind stageKind;
        switch (req.request.shader.stage) {
            case ShaderStage::Compute: stageKind = ShaderStageKind::Compute; break;
            case ShaderStage::Vertex: stageKind = ShaderStageKind::Vertex; break;
            case ShaderStage::TessellationControl: stageKind = ShaderStageKind::TessellationControl; break;
            case ShaderStage::TessellationEvaluation: stageKind = ShaderStageKind::TessellationEvaluation; break;
            case ShaderStage::Fragment: stageKind = ShaderStageKind::Pixel; break;
            case ShaderStage::Local: stageKind = ShaderStageKind::Local; break;
            case ShaderStage::Mesh: stageKind = ShaderStageKind::Mesh; break;
            default: throw std::runtime_error("unsupported stage");
        }
        const auto inputInfo = BuildShaderStageInputInfo(stageKind, req.request.context);

        GraphBuilder graphBuilder;
        auto cfg = graphBuilder.Build(decoded);
        Structurizer structurizer;
        structurizer.Structurize(cfg);

        TranslateOptions translateOptions{};
        translateOptions.stage = stageKind;
        translateOptions.waveSize = req.request.context.waveSize;
        translateOptions.userDataBaseRegister = req.request.context.userDataBaseRegister;
        translateOptions.userDataCount = static_cast<std::uint32_t>(req.request.context.userData.size());
        translateOptions.embeddedFetch = nullptr;
        translateOptions.inputInfo = inputInfo;

        InstructionTranslator translator;
        auto program = translator.Translate(decoded, cfg, translateOptions);
        std::cout << "Translate OK, blocks=" << program.Blocks().size() << "\n";

        SsaBuilder ssaBuilder;
        ssaBuilder.Rewrite(program);
        std::cout << "SSA OK\n";

        ConstantFolder constantFolder;
        DeadCodeEliminator deadCodeEliminator;
        constantFolder.Fold(program);
        ResolveControlFlowIdentities(program);
        deadCodeEliminator.RemoveIdentities(program);
        deadCodeEliminator.Eliminate(program);
        std::cout << "Fold/DCE pass 1 OK\n";

        ReadLaneEliminator readLaneEliminator;
        const auto readLaneStats = readLaneEliminator.Eliminate(program, translateOptions.waveSize);
        std::cout << "ReadLaneEliminator rewrote " << readLaneStats.rewrittenReads << "\n";
        if (readLaneStats.rewrittenReads != 0u) {
            constantFolder.Fold(program);
            ResolveControlFlowIdentities(program);
            deadCodeEliminator.RemoveIdentities(program);
            deadCodeEliminator.Eliminate(program);
        }

        SrtWalker srtWalker;
        srtWalker.BuildPlan(program);
        std::cout << "SrtWalker::BuildPlan OK\n";
        deadCodeEliminator.Eliminate(program);
        std::cout << "DCE after SRT OK\n";

        ResourceTracker resourceTracker;
        resourceTracker.Track(program);
        std::cout << "ResourceTracker::Track OK\n";

    } catch (const std::exception& e) {
        std::cerr << "PIPELINE FAILED: " << e.what() << "\n";
        return 2;
    }

    std::cout << "ALL STAGES OK\n";
    return 0;
}
