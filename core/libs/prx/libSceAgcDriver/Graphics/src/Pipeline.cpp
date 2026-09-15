#include "prx/libSceAgcDriver/Graphics/include/Pipeline.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace AgcDriver::Graphics {

Pipeline::Pipeline(const Context& context, const State& state, const RenderTarget& target, const ShaderResources& resources, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment) : context(context) {
    ValidateShaderPair(vertex, fragment);
    const auto& viewport = state.viewport;
    Require(std::isfinite(viewport.x) && std::isfinite(viewport.y) && std::isfinite(viewport.width) && std::isfinite(viewport.height), "viewport arithmetic overflow");
    Require(viewport.width <= context.limits.maxViewportDimensions[0] && std::abs(viewport.height) <= context.limits.maxViewportDimensions[1], "viewport dimensions exceed device limits");
    Require(viewport.x >= context.limits.viewportBoundsRange[0] && viewport.x + viewport.width <= context.limits.viewportBoundsRange[1], "viewport X exceeds device bounds");
    Require(std::min(viewport.y, viewport.y + viewport.height) >= context.limits.viewportBoundsRange[0] && std::max(viewport.y, viewport.y + viewport.height) <= context.limits.viewportBoundsRange[1], "viewport Y exceeds device bounds");
    Require(context.limits.maxPushConstantsSize >= 2 * StagePushConstantBytes, "graphics push constant range exceeds device limit");
    const std::array<const ShaderRecompiler::RecompileResult*, 2> shaders{&vertex, &fragment};
    try {
        std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
        std::vector<VkPushConstantRange> pushes;
        for (std::uint32_t i = 0; i < shaders.size(); ++i) {
            const auto& shader = *shaders[i];
            Require(shader.pushConstants.size() <= StagePushConstantBytes && shader.pushConstants.size() % 4 == 0, "shader push constants exceed the assigned stage range");
            VkShaderModuleCreateInfo module{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
            module.codeSize = shader.spirv.size() * sizeof(std::uint32_t);
            module.pCode = shader.spirv.data();
            Check(context.Function<PFN_vkCreateShaderModule>("vkCreateShaderModule")(context.device, &module, nullptr, &modules[i]), "vkCreateShaderModule graphics");
            const auto stage = i == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
            stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stages[i].stage = stage;
            stages[i].module = modules[i];
            stages[i].pName = "main";
            if (!shader.pushConstants.empty()) pushes.push_back({stage, i * StagePushConstantBytes, static_cast<std::uint32_t>(shader.pushConstants.size())});
        }
        VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        layoutInfo.setLayoutCount = static_cast<std::uint32_t>(resources.Layouts().size());
        layoutInfo.pSetLayouts = resources.Layouts().data();
        layoutInfo.pushConstantRangeCount = static_cast<std::uint32_t>(pushes.size());
        layoutInfo.pPushConstantRanges = pushes.data();
        Check(context.Function<PFN_vkCreatePipelineLayout>("vkCreatePipelineLayout")(context.device, &layoutInfo, nullptr, &layout), "vkCreatePipelineLayout graphics");
        VkAttachmentDescription color{};
        color.format = state.color.format;
        color.samples = VK_SAMPLE_COUNT_1_BIT;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference reference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &reference;
        VkRenderPassCreateInfo passInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        passInfo.attachmentCount = 1;
        passInfo.pAttachments = &color;
        passInfo.subpassCount = 1;
        passInfo.pSubpasses = &subpass;
        Check(context.Function<PFN_vkCreateRenderPass>("vkCreateRenderPass")(context.device, &passInfo, nullptr, &renderPass), "vkCreateRenderPass");
        const auto view = target.View();
        VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &view;
        framebufferInfo.width = state.color.extent.width;
        framebufferInfo.height = state.color.extent.height;
        framebufferInfo.layers = 1;
        Check(context.Function<PFN_vkCreateFramebuffer>("vkCreateFramebuffer")(context.device, &framebufferInfo, nullptr, &framebuffer), "vkCreateFramebuffer");
        VkPipelineVertexInputStateCreateInfo input{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
        VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        assembly.topology = state.topology;
        VkPipelineViewportStateCreateInfo viewports{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewports.viewportCount = 1;
        viewports.pViewports = &state.viewport;
        viewports.scissorCount = 1;
        viewports.pScissors = &state.scissor;
        VkPipelineRasterizationStateCreateInfo raster{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        raster.polygonMode = VK_POLYGON_MODE_FILL;
        raster.cullMode = state.cullMode;
        raster.frontFace = state.frontFace;
        raster.lineWidth = 1;
        VkPipelineMultisampleStateCreateInfo samples{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        samples.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkPipelineColorBlendStateCreateInfo blend{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        blend.attachmentCount = 1;
        blend.pAttachments = &state.blend;
        std::copy(state.blendConstants.begin(), state.blendConstants.end(), blend.blendConstants);
        VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipelineInfo.stageCount = static_cast<std::uint32_t>(stages.size());
        pipelineInfo.pStages = stages.data();
        pipelineInfo.pVertexInputState = &input;
        pipelineInfo.pInputAssemblyState = &assembly;
        pipelineInfo.pViewportState = &viewports;
        pipelineInfo.pRasterizationState = &raster;
        pipelineInfo.pMultisampleState = &samples;
        pipelineInfo.pColorBlendState = &blend;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = renderPass;
        Check(context.Function<PFN_vkCreateGraphicsPipelines>("vkCreateGraphicsPipelines")(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "vkCreateGraphicsPipelines");
    } catch (...) {
        release();
        throw;
    }
}

Pipeline::~Pipeline() {
    release();
}

void Pipeline::release() noexcept {
    if (pipeline) context.Function<PFN_vkDestroyPipeline>("vkDestroyPipeline")(context.device, pipeline, nullptr);
    if (framebuffer) context.Function<PFN_vkDestroyFramebuffer>("vkDestroyFramebuffer")(context.device, framebuffer, nullptr);
    if (renderPass) context.Function<PFN_vkDestroyRenderPass>("vkDestroyRenderPass")(context.device, renderPass, nullptr);
    if (layout) context.Function<PFN_vkDestroyPipelineLayout>("vkDestroyPipelineLayout")(context.device, layout, nullptr);
    for (auto module : modules) {
        if (module) context.Function<PFN_vkDestroyShaderModule>("vkDestroyShaderModule")(context.device, module, nullptr);
    }
}

VkPipelineLayout Pipeline::Layout() const {
    return layout;
}

void Pipeline::Begin(VkCommandBuffer commands, VkExtent2D extent) const {
    VkRenderPassBeginInfo begin{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    begin.renderPass = renderPass;
    begin.framebuffer = framebuffer;
    begin.renderArea = {{0, 0}, extent};
    context.Function<PFN_vkCmdBeginRenderPass>("vkCmdBeginRenderPass")(commands, &begin, VK_SUBPASS_CONTENTS_INLINE);
    context.Function<PFN_vkCmdBindPipeline>("vkCmdBindPipeline")(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void Pipeline::PushConstants(VkCommandBuffer commands, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment) const {
    const std::array<const ShaderRecompiler::RecompileResult*, 2> shaders{&vertex, &fragment};
    for (std::uint32_t i = 0; i < shaders.size(); ++i) {
        const auto& bytes = shaders[i]->pushConstants;
        if (bytes.empty()) continue;
        const auto stage = i == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        context.Function<PFN_vkCmdPushConstants>("vkCmdPushConstants")(commands, layout, stage, i * StagePushConstantBytes, static_cast<std::uint32_t>(bytes.size()), bytes.data());
    }
}

}
