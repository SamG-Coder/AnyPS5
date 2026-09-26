#include "prx/libSceAgcDriver/Graphics/include/NativeDrawCompiler.hpp"
#include "prx/libSceAgcDriver/Execution/include/VulkanDevice.hpp"
#include "prx/libSceAgcDriver/Execution/include/ShaderMemory.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ShaderInputState.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Shaders.hpp"
#include "prx/libc/include/General.hpp"
namespace AgcDriver::Graphics {
void CompileAndEnqueueNativeDraw(VulkanDevice& device, const State& graphics, DrawParameters draw,
    std::span<const NativeShaderProgram> programs,
    const ShaderRecompiler::ShaderPixelStageInfo& pixel,
    std::span<const ShaderRecompiler::MemoryRegion> initialMemory) {
    using Stage=ShaderRecompiler::ShaderStage;
    std::vector<ShaderRecompiler::MemoryRegion> memory(initialMemory.begin(),initialMemory.end());
    std::vector<ShaderRecompiler::LinkedProgram> linked;
    linked.reserve(programs.size());
    for(const auto& p:programs) linked.push_back({p.role,p.binary,p.userDataBase,p.firstUserSgpr,p.userData});
    ShaderMemory shaderMemory(memory);
    std::vector<ShaderRecompiler::RecompileResult> results;
    std::vector<CompiledShader> stages;
    results.reserve(programs.size()+(graphics.rectList?2u:0u));
    stages.reserve(programs.size()+2u);
    std::uint32_t pushCursor=0;
    for(std::size_t i=0;i<programs.size();++i){
        const auto& p=programs[i];
        if(p.role==ShaderRecompiler::ProgramRole::GeometryBack) continue;
        const auto wave=p.binary.stage==Stage::Fragment?graphics.stages.fragmentWaveSize:graphics.stages.vertexWaveSize;
        ShaderRecompiler::RecompileRequest request{
            p.binary,
            {wave,p.firstUserSgpr,p.userData,std::nullopt,
             p.binary.stage==Stage::Fragment?std::optional(pixel):std::nullopt,
             p.binary.stage==Stage::Fragment?std::nullopt:std::optional(DecodeVertexStageInfo(p.binary.header,p.binary.headerAddress,p.userData)),
             memory},
            device.Target(),
            {0,0,pushCursor,PipelinePushConstantBytes-pushCursor},
            ShaderRecompiler::GraphicsCompileContext{p.firstUserSgpr,linked,graphics.stages.mesh,graphics.stages.tessellation,
                {draw.indexAddress,draw.indexCount,draw.indexSize,draw.instanceCount}}
        };
        shaderMemory.Capture(request,p.missingUserData);
        memory=shaderMemory.Regions();
        request.context.memory=memory;
        results.push_back(ShaderRecompiler::Recompile(request));
        auto& result=results.back();
        if(!draw.indexed&&i==0){
            const auto offsetValue=[&](std::int32_t sgpr){
                Require(sgpr>=0&&static_cast<std::uint32_t>(sgpr)>=p.firstUserSgpr,"invalid native draw offset SGPR");
                const auto index=static_cast<std::uint32_t>(sgpr)-p.firstUserSgpr;
                Require(index<p.userData.size(),"native draw offset SGPR exceeds user data");
                return p.userData[index];
            };
            if(draw.firstVertex==0&&result.vertexOffsetSgpr>=0) draw.firstVertex=offsetValue(result.vertexOffsetSgpr);
            if(result.instanceOffsetSgpr>=0) draw.firstInstance=offsetValue(result.instanceOffsetSgpr);
        }
        Require(result.pushConstants.size()<=PipelinePushConstantBytes-pushCursor,"native shader push constants exceed pipeline block");
        stages.push_back({p.binary.stage,&result,result.pushConstants.empty()?0u:pushCursor});
        pushCursor+=static_cast<std::uint32_t>(result.pushConstants.size());
    }
    if(graphics.rectList){
        Require(stages.size()==2,"native rect-list requires vertex and fragment programs");
        auto rectangle=ShaderRecompiler::BuildRectListShaders(results[0],results[1],device.Target());
        results.push_back(std::move(rectangle.control)); results.push_back(std::move(rectangle.evaluation));
        stages.insert(stages.begin()+1,{{Stage::TessellationControl,&results[results.size()-2],0},{Stage::TessellationEvaluation,&results.back(),0}});
    }
    std::vector<GuestMemorySnapshot> snapshots;
    for(const auto& region:memory) snapshots.push_back({region.guestAddress,region.bytes});
    device.EnqueueDraw(graphics,draw,stages,snapshots);
}
}
