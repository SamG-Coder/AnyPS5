#include "prx/libSceAgcDriver/Graphics/include/NativeGraphicsState.hpp"
#include "prx/libc/include/General.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ColorTargetLayout.hpp"
#include "prx/libSceAgcDriver/Graphics/include/DepthTargetLayout.hpp"
#include <bit>
#include <cmath>
#include <array>
#include <bit>
#include <stdexcept>
namespace AgcDriver::Graphics {
std::optional<ShaderRecompiler::ShaderPixelStageInfo> NativeGraphicsState::PixelStage() const {
    if(!psInputControl||!psInputEnable||!psInputAddress||!dbShaderControl||!shaderColorFormat) return std::nullopt;
    const auto inputNum=*psInputControl&0x3fu;
    if(inputNum>32) throw std::runtime_error("native AGC: pixel interpolator count exceeds 32");
    std::array<std::uint32_t,32> settings{};
    for(std::uint32_t i=0;i<inputNum;++i){if(!interpolants[i]) return std::nullopt; settings[i]=*interpolants[i];}
    const auto active=*psInputEnable&*psInputAddress;
    constexpr std::uint32_t known=0x1u|0x2u|0x10u|0x20u|0x80u|0x100u|0x200u|0x400u|0x800u|0x1000u|0x2000u;
    if(active&~known) throw std::runtime_error("native AGC: unsupported pixel input state");
    std::array<std::uint8_t,8> modes{},mapping{}; mapping.fill(0xe4u);
    for(std::uint32_t i=0;i<8;++i)modes[i]=static_cast<std::uint8_t>((*shaderColorFormat>>(4u*i))&0xfu);
    if(state.hasColorTarget) mapping[0]=state.color.componentMapping;
    const bool perspective=(active&0x2u)!=0, kill=(*dbShaderControl&0x40u)!=0, depth=(*dbShaderControl&1u)!=0, mask=(*dbShaderControl&0x100u)!=0;
    const auto z=(*dbShaderControl>>4u)&3u;
    return ShaderRecompiler::ShaderPixelStageInfo{inputNum,settings,(*psInputControl&0x8000u)!=0,
        perspective?((active&1u)?2u:0u):0u,perspective,(active&0x100u)!=0,(active&0x200u)!=0,(active&0x400u)!=0,(active&0x800u)!=0,
        (active&0x1000u)!=0,(active&0x2000u)!=0,(active&0x11u)==0x11u,(active&0x20u)!=0,kill,depth,mask,
        z==1u&&!kill&&!depth&&!mask,(*dbShaderControl&0x400u)!=0,modes,mapping,
        2u*static_cast<std::uint32_t>(std::popcount(active&0x33u))+((active&0x80u)?1u:0u),(active&0x80u)!=0};
}
bool NativeGraphicsState::ReadyForDraw() const {
    if (!primitive || !raster || !clipControl) return false;
    for (const auto& v:viewport) if (!v) return false;
    if (!screenTl || !screenBr) return false;
    if (state.hasColorTarget) {
        if (!targetMask || !shaderMask || !colorControl || !colorInfo || !colorBase || !colorBaseExt || !colorAttrib2 || !colorAttrib3) return false;
    }
    return state.renderExtent.width!=0 && state.renderExtent.height!=0;
}
void NativeGraphicsState::SetUser(std::uint32_t o, std::uint32_t v) {
    if (o == 0x242) { primitive=v; updateTopology(); }
}
void NativeGraphicsState::SetContext(std::uint32_t o, std::uint32_t v) {
    if (o == 0x205) { raster=v; updateRaster(); return; }
    if (o == 0x200) depthControl=v;
    else if (o == 0x0) depthRenderControl=v;
    else if (o == 0x2) depthView=v;
    else if (o == 0x10) depthInfo=v;
    else if (o == 0x7) depthSize=v;
    else if (o == 0x12) depthReadBase=v;
    else if (o == 0x1a) depthReadBaseExt=v;
    else if (o == 0x14) depthWriteBase=v;
    else if (o == 0x1c) depthWriteBaseExt=v;
    else goto not_depth;
    if(depthControl && (*depthControl&2u)==0) { state.depth.reset(); return; }
    if(depthControl&&depthRenderControl&&depthView&&depthInfo&&depthSize&&depthReadBase&&depthReadBaseExt){
        if((*depthControl&~0x007007f6u)!=0 || *depthRenderControl!=0 || *depthView!=0 || *depthInfo!=0x80000183u)
            throw std::runtime_error("native AGC: unsupported depth configuration");
        const auto sz=*depthSize; if(sz&0xc000c000u) throw std::runtime_error("native AGC: invalid depth extent");
        const VkExtent2D extent{(sz&0x3fffu)+1u,((sz>>16u)&0x3fffu)+1u};
        if((*depthReadBaseExt&~0xffu)!=0) throw std::runtime_error("native AGC: invalid depth address extension");
        const auto address=(static_cast<std::uint64_t>(*depthReadBaseExt)<<40u)|(static_cast<std::uint64_t>(*depthReadBase)<<8u);
        if(!address||(address&0xffffu)) throw std::runtime_error("native AGC: depth address must be 64KB aligned");
        const bool write=(*depthControl&4u)!=0;
        if(write){
            if(!depthWriteBase||!depthWriteBaseExt) return;
            const auto writeAddress=(static_cast<std::uint64_t>(*depthWriteBaseExt)<<40u)|(static_cast<std::uint64_t>(*depthWriteBase)<<8u);
            if(writeAddress!=address) throw std::runtime_error("native AGC: separate depth read/write surfaces unsupported");
        }
        constexpr VkCompareOp cmp[]{VK_COMPARE_OP_NEVER,VK_COMPARE_OP_LESS,VK_COMPARE_OP_EQUAL,VK_COMPARE_OP_LESS_OR_EQUAL,VK_COMPARE_OP_GREATER,VK_COMPARE_OP_NOT_EQUAL,VK_COMPARE_OP_GREATER_OR_EQUAL,VK_COMPARE_OP_ALWAYS};
        state.depth=DepthState{address,extent,DepthTargetLayout(extent.width,extent.height).Bytes(),cmp[(*depthControl>>4u)&7u],write};
        if(!state.hasColorTarget) state.renderExtent=extent;
    }
    return;
not_depth:
    if (o == 0x1b6) { psInputControl=v; return; }
    if (o == 0x1b3) { psInputEnable=v; return; }
    if (o == 0x1b4) { psInputAddress=v; return; }
    if (o == 0x203) { dbShaderControl=v; return; }
    if (o == 0x1c5) { shaderColorFormat=v; return; }
    if (o >= 0x191 && o < 0x191+32) { interpolants[o-0x191]=v; return; }
    if (o == 0x206) { viewportControl=v; return; }
    if (o == 0x204) { clipControl=v; state.negativeOneToOne=(v&0x80000u)==0; updateViewport(); return; }
    if (o >= 0x10f && o <= 0x114) { viewport[o-0x10f]=v; updateViewport(); return; }
    if (o == 0xc) screenTl=v;
    else if (o == 0xd) screenBr=v;
    else if (o == 0x81) windowTl=v;
    else if (o == 0x82) windowBr=v;
    else if (o == 0x8e) { targetMask=v; updateColorTarget(); updateBlend(); return; }
    else if (o == 0x8f) { shaderMask=v; updateColorTarget(); return; }
    else if (o == 0x202) { colorControl=v; updateColorTarget(); return; }
    else if (o == 0x318) { colorBase=v; updateColorTarget(); return; }
    else if (o == 0x390) { colorBaseExt=v; updateColorTarget(); return; }
    else if (o == 0x31c) { colorInfo=v; updateColorTarget(); updateBlend(); return; }
    else if (o == 0x3b0) { colorAttrib2=v; updateColorTarget(); return; }
    else if (o == 0x3b8) { colorAttrib3=v; updateColorTarget(); return; }
    else if (o == 0x1e0) { blendControl=v; updateBlend(); return; }
    else if (o >= 0x105 && o <= 0x108) { blendConstant[o-0x105]=v; updateBlend(); return; }
    else return;
    updateScissor();
}
void NativeGraphicsState::updateTopology() {
    if (!primitive) return;
    state.rectList=false;
    switch (*primitive) {
        case 1: state.topology=VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case 2: state.topology=VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
        case 4: state.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case 5: state.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
        case 6: state.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        case 7: case 17: state.rectList=true; state.topology=VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
        case 9: state.topology=VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
        default: throw std::runtime_error("native AGC: unsupported primitive topology");
    }
}
void NativeGraphicsState::updateRaster() {
    if (!raster) return;
    const auto v=*raster;
    if ((v & ~0x80007u) != 0 && (v & ~0x80007u) != 0x240u) throw std::runtime_error("native AGC: unsupported raster state");
    state.provokingVertexLast=(v&0x80000u)!=0;
    state.cullMode=((v&1u)?VK_CULL_MODE_FRONT_BIT:0u)|((v&2u)?VK_CULL_MODE_BACK_BIT:0u);
    state.frontFace=(v&4u)?VK_FRONT_FACE_CLOCKWISE:VK_FRONT_FACE_COUNTER_CLOCKWISE;
}
void NativeGraphicsState::updateViewport() {
    if (!clipControl) return;
    for (const auto& v:viewport) if (!v) return;
    const auto f=[](std::uint32_t v){auto x=std::bit_cast<float>(v); if(!std::isfinite(x)) throw std::runtime_error("native AGC: non-finite viewport"); return x;};
    const float xs=f(*viewport[0]), xo=f(*viewport[1]), ys=f(*viewport[2]), yo=f(*viewport[3]), zs=f(*viewport[4]), zo=f(*viewport[5]);
    if (!(xs>0 && ys!=0)) throw std::runtime_error("native AGC: unsupported viewport transform");
    const float minDepth=state.negativeOneToOne?zo-zs:zo, maxDepth=zo+zs;
    state.viewport={xo-xs,yo-ys,2*xs,2*ys,minDepth,maxDepth};
}
static VkBlendFactor nativeBlendFactor(std::uint32_t value) {
    switch(value) {
        case 0:return VK_BLEND_FACTOR_ZERO; case 1:return VK_BLEND_FACTOR_ONE;
        case 2:return VK_BLEND_FACTOR_SRC_COLOR; case 3:return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case 4:return VK_BLEND_FACTOR_SRC_ALPHA; case 5:return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case 6:return VK_BLEND_FACTOR_DST_ALPHA; case 7:return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case 8:return VK_BLEND_FACTOR_DST_COLOR; case 9:return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case 10:return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; case 13:return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case 14:return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR; case 19:return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case 20:return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        default: throw std::runtime_error("native AGC: unsupported blend factor");
    }
}
static VkBlendOp nativeBlendOp(std::uint32_t value) {
    switch(value){case 0:return VK_BLEND_OP_ADD;case 1:return VK_BLEND_OP_SUBTRACT;case 2:return VK_BLEND_OP_MIN;case 3:return VK_BLEND_OP_MAX;case 4:return VK_BLEND_OP_REVERSE_SUBTRACT;default:throw std::runtime_error("native AGC: unsupported blend operation");}
}
void NativeGraphicsState::updateColorTarget() {
    if (!targetMask) return;
    state.hasColorTarget=*targetMask!=0;
    if (!state.hasColorTarget) return;
    if (!shaderMask || !colorControl || !colorInfo || !colorBase || !colorBaseExt || !colorAttrib2 || !colorAttrib3) return;
    if ((*targetMask & ~0xfu)!=0 || *shaderMask!=0xfu) throw std::runtime_error("native AGC: unsupported color target mask");
    if ((*colorControl & ~1u)!=0xcc0010u) throw std::runtime_error("native AGC: unsupported color control");
    const auto info=*colorInfo, number=(info>>8u)&7u, swap=(info>>11u)&3u;
    if (((info>>2u)&0x1fu)!=10 || (number!=0&&number!=6) || swap>1 || (info&0x8000u)==0) throw std::runtime_error("native AGC: unsupported color target format");
    state.color.tileMode=DecodeColorTileMode(*colorAttrib3);
    state.color.extent={((*colorAttrib2>>14u)&0x3fffu)+1u,(*colorAttrib2&0x3fffu)+1u};
    const ColorTargetLayout layout(state.color.extent.width,state.color.extent.height,state.color.tileMode);
    if ((*colorBaseExt & ~0xffu)!=0) throw std::runtime_error("native AGC: invalid color address extension");
    state.color.address=(static_cast<std::uint64_t>(*colorBaseExt)<<40u)|(static_cast<std::uint64_t>(*colorBase)<<8u);
    state.color.bytes=layout.Bytes();
    state.color.format=swap==0?(number==0?VK_FORMAT_R8G8B8A8_UNORM:VK_FORMAT_R8G8B8A8_SRGB):(number==0?VK_FORMAT_B8G8R8A8_UNORM:VK_FORMAT_B8G8R8A8_SRGB);
    state.color.componentMapping=0xe4u;
    state.renderExtent=state.color.extent;
}
void NativeGraphicsState::updateBlend() {
    if (!targetMask || !blendControl || *targetMask==0) return;
    const auto v=*blendControl;
    if ((v&0x0000e000u)!=0) throw std::runtime_error("native AGC: reserved blend bits");
    state.blend.colorWriteMask=*targetMask;
    state.blend.blendEnable=(v>>30u)&1u;
    if (!state.blend.blendEnable) return;
    if (colorInfo && (*colorInfo&0x10000u)!=0) throw std::runtime_error("native AGC: blend bypass conflict");
    state.blend.srcColorBlendFactor=nativeBlendFactor(v&0x1fu);
    state.blend.dstColorBlendFactor=nativeBlendFactor((v>>8u)&0x1fu);
    state.blend.colorBlendOp=nativeBlendOp((v>>5u)&7u);
    const auto alpha=(v&0x20000000u)?v>>16u:v;
    state.blend.srcAlphaBlendFactor=nativeBlendFactor(alpha&0x1fu);
    state.blend.dstAlphaBlendFactor=nativeBlendFactor((alpha>>8u)&0x1fu);
    state.blend.alphaBlendOp=nativeBlendOp((alpha>>5u)&7u);
    for(std::uint32_t i=0;i<4;++i) if(blendConstant[i]) state.blendConstants[i]=std::bit_cast<float>(*blendConstant[i]);
}
void NativeGraphicsState::updateScissor() {
    if (!screenTl || !screenBr) return;
    const auto tl=*screenTl, br=*screenBr;
    const auto x=tl&0xffffu, y=tl>>16u, right=br&0xffffu, bottom=br>>16u;
    if(x>right||y>bottom) throw std::runtime_error("native AGC: inverted screen scissor");
    state.scissor={{static_cast<std::int32_t>(x),static_cast<std::int32_t>(y)},{right-x,bottom-y}};
    state.renderExtent={right,bottom};
}
}
