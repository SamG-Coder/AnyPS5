#include "prx/libSceAgcDriver/Graphics/include/NativeGraphicsState.hpp"
#include "prx/libc/include/General.hpp"
#include <bit>
#include <cmath>
#include <stdexcept>
namespace AgcDriver::Graphics {
void NativeGraphicsState::SetUser(std::uint32_t o, std::uint32_t v) {
    if (o == 0x242) { primitive=v; updateTopology(); }
}
void NativeGraphicsState::SetContext(std::uint32_t o, std::uint32_t v) {
    if (o == 0x205) { raster=v; updateRaster(); return; }
    if (o == 0x206) { viewportControl=v; return; }
    if (o == 0x204) { clipControl=v; state.negativeOneToOne=(v&0x80000u)==0; updateViewport(); return; }
    if (o >= 0x10f && o <= 0x114) { viewport[o-0x10f]=v; updateViewport(); return; }
    if (o == 0xc) screenTl=v;
    else if (o == 0xd) screenBr=v;
    else if (o == 0x81) windowTl=v;
    else if (o == 0x82) windowBr=v;
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
void NativeGraphicsState::updateScissor() {
    if (!screenTl || !screenBr) return;
    const auto tl=*screenTl, br=*screenBr;
    const auto x=tl&0xffffu, y=tl>>16u, right=br&0xffffu, bottom=br>>16u;
    if(x>right||y>bottom) throw std::runtime_error("native AGC: inverted screen scissor");
    state.scissor={{static_cast<std::int32_t>(x),static_cast<std::int32_t>(y)},{right-x,bottom-y}};
    state.renderExtent={right,bottom};
}
}
