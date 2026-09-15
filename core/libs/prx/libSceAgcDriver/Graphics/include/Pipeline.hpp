#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_PIPELINE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_PIPELINE_HPP

#include "prx/libSceAgcDriver/Graphics/include/ShaderResources.hpp"

namespace AgcDriver::Graphics {

inline constexpr std::uint32_t StagePushConstantBytes = 64;

class Pipeline {
public:
    Pipeline(const Context& context, const State& state, const RenderTarget& target, const ShaderResources& resources, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment);
    ~Pipeline();
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    VkPipelineLayout Layout() const;
    void Begin(VkCommandBuffer commands, VkExtent2D extent) const;
    void PushConstants(VkCommandBuffer commands, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment) const;

private:
    void release() noexcept;
    const Context& context;
    std::array<VkShaderModule, 2> modules{};
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
};

void ValidateShaderPair(const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment);

}

#endif
