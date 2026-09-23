#include "prx/libSceAgcDriver/Graphics/include/Draw.hpp"
#include "prx/libSceAgcDriver/Graphics/include/ColorTargetTransfer.hpp"
#include "prx/libSceAgcDriver/Graphics/include/GpuColorTransfer.hpp"
#include "prx/libSceAgcDriver/Graphics/include/VertexInput.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libSceAgcDriver/Execution/include/PerformanceTimer.hpp"
#include "prx/libc/include/General.hpp"
#include <cstring>
#include <limits>
#include <memory>

namespace AgcDriver::Graphics {
namespace {

void imageBarrier(const Context& context, VkCommandBuffer commands, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags sourceAccess, VkAccessFlags destinationAccess) {
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.srcAccessMask = sourceAccess;
    barrier.dstAccessMask = destinationAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

}

void Draw(const Context& context, const State& state, const Pm4::DrawParameters& draw, std::span<const CompiledShader> shaders, std::span<const GuestMemorySnapshot> snapshots) {
    PerformanceTimer timing("Graphics.Draw");
    APS5_LOG_OUT("Draw indices=%u instances=%u indexSize=%u flags=%u indexAddress=0x%llx", draw.indexCount, draw.instanceCount, draw.indexSize, draw.flags, static_cast<unsigned long long>(draw.indexAddress));
    APS5_LOG_OUT_DEBUG("State colorTarget=%u render=%ux%u colorAddress=0x%llx colorBytes=%llu colorExtent=%ux%u", state.hasColorTarget ? 1u : 0u, state.renderExtent.width, state.renderExtent.height, static_cast<unsigned long long>(state.color.address), static_cast<unsigned long long>(state.color.bytes), state.color.extent.width, state.color.extent.height);
    APS5_LOG_OUT_DEBUG("Viewport x=%f y=%f w=%f h=%f minDepth=%f maxDepth=%f", state.viewport.x, state.viewport.y, state.viewport.width, state.viewport.height, state.viewport.minDepth, state.viewport.maxDepth);
    APS5_LOG_OUT_DEBUG("Scissor x=%d y=%d w=%u h=%u topology=%u cullMode=0x%x frontFace=%u", state.scissor.offset.x, state.scissor.offset.y, state.scissor.extent.width, state.scissor.extent.height, static_cast<unsigned>(state.topology), static_cast<unsigned>(state.cullMode), static_cast<unsigned>(state.frontFace));
    Require(draw.indexed ? draw.flags == 0 : (draw.flags & ~0x20u) == 0, "draw modifiers are unsupported");
    if (draw.indexed) {
        Require(draw.indexSize == 2 || draw.indexSize == 4, "only uint16 and uint32 index buffers are supported");
        Require(draw.firstVertex == 0 && draw.firstInstance == 0, "indexed draw offsets are unsupported");
    } else {
        Require(draw.indexAddress == 0 && draw.indexSize == 0, "auto draw must not reference an index buffer");
        if (draw.indexCount == 0 || draw.instanceCount == 0) return;
        Require(draw.firstVertex <= std::numeric_limits<std::uint32_t>::max() - (draw.indexCount - 1u), "auto draw vertex range overflow");
        Require(draw.firstInstance <= std::numeric_limits<std::uint32_t>::max() - (draw.instanceCount - 1u), "auto draw instance range overflow");
    }
    Require(draw.indexCount != 0 && draw.instanceCount != 0, "zero-count indexed draws are unsupported");
    const auto indexBytes = static_cast<std::uint64_t>(draw.indexCount) * draw.indexSize;
    APS5_LOG_OUT_DEBUG("Index buffer bytes=%llu", static_cast<unsigned long long>(indexBytes));
    Require(indexBytes <= std::numeric_limits<std::size_t>::max(), "index buffer size overflow");
    if (draw.indexed) GuestMemory::CheckRange(reinterpret_cast<const void*>(draw.indexAddress), static_cast<std::size_t>(indexBytes), draw.indexSize);
    APS5_LOG_CHARS_OUT_DEBUG("Index buffer range OK");
    Require(!draw.indexed || !state.hasColorTarget || draw.indexAddress + indexBytes <= state.color.address || state.color.address + state.color.bytes <= draw.indexAddress, "index buffer aliases the render target");
    if (state.rectList) Require(draw.indexCount % 3 == 0, "incomplete rect-list primitive");
    APS5_LOG_CHARS_OUT_DEBUG("ValidateShaders");
    ValidateShaders(shaders, state, context.subgroup, context.fragmentShaderBarycentric);
    APS5_LOG_CHARS_OUT_DEBUG("ValidateShaders OK");
    const auto shaderStages = PipelineStages(shaders);
    APS5_LOG_OUT_DEBUG("PipelineStages=0x%x", static_cast<unsigned>(shaderStages));
    std::uint32_t meshGroups = 0;
    if (state.stages.mesh) {
        Require(draw.firstVertex == 0 && draw.firstInstance == 0, "mesh draw offsets are unsupported");
        APS5_LOG_CHARS_OUT_DEBUG("Mesh path");
        Require(context.meshShader, "device does not support mesh shaders");
        const auto& mesh = *state.stages.mesh;
        const auto inputSize = mesh.inputPrimitive == 1 ? 1u : mesh.inputPrimitive == 2 ? 2u : 3u;
        Require(draw.indexCount >= inputSize && mesh.primitivesPerGroup != 0, "mesh draw contains no complete primitive");
        const auto step = mesh.inputPrimitive == 6 ? 1u : inputSize;
        const auto primitives = (draw.indexCount - inputSize) / step + 1u;
        meshGroups = (primitives - 1u) / mesh.primitivesPerGroup + 1u;
        APS5_LOG_OUT_DEBUG("Mesh primitives=%u groups=%u", primitives, meshGroups);
        Require(meshGroups <= context.meshLimits.maxMeshWorkGroupCount[0] && draw.instanceCount <= context.meshLimits.maxMeshWorkGroupCount[1] && static_cast<std::uint64_t>(meshGroups) * draw.instanceCount <= context.meshLimits.maxMeshWorkGroupTotalCount, "mesh draw exceeds workgroup count limits");
    }
    if (state.stages.tessellation) Require(draw.indexCount % state.stages.tessellation->inputControlPoints == 0, "incomplete tessellation patch");
    timing.Mark("validate");
    std::unique_ptr<Buffer> indices;
    std::uint32_t maxIndex = draw.indexed ? 0u : draw.firstVertex + draw.indexCount - 1u;
    if (draw.indexed) {
        indices = std::make_unique<Buffer>(context, static_cast<std::size_t>(indexBytes), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        GuestMemory::Read(draw.indexAddress, indices->Bytes(), draw.indexSize);
        for (std::size_t offset = 0; offset < indexBytes; offset += draw.indexSize) {
            std::uint32_t index = 0;
            if (draw.indexSize == 2) {
                std::uint16_t value = 0;
                std::memcpy(&value, indices->Bytes().data() + offset, sizeof(value));
                index = value;
            } else {
                std::memcpy(&index, indices->Bytes().data() + offset, sizeof(index));
            }
            Require(index <= context.limits.maxDrawIndexedIndexValue, "index exceeds the device's indexed draw limit");
            maxIndex = std::max(maxIndex, index);
        }
    }
    APS5_LOG_CHARS_OUT_DEBUG("Index validation OK");
    timing.Mark("index_upload");
    const auto& attributes = shaders.front().program->vertexAttributes;
    static_cast<void>(BuildVertexInputLayout(context, attributes));
    std::vector<std::unique_ptr<Buffer>> vertexBuffers;
    std::vector<VkBuffer> vertexHandles;
    std::vector<VkDeviceSize> vertexOffsets(attributes.size(), 0);
    for (const auto& attribute : attributes) {
        const auto bytes = VertexBufferReadSize(attribute, maxIndex, draw.instanceCount, draw.firstInstance);
        const auto& fields = attribute.resource.fields;
        const auto address = fields[0] | (static_cast<std::uint64_t>(fields[1] & 0xffffu) << 32u);
        Require(!state.hasColorTarget || address + bytes <= state.color.address || state.color.address + state.color.bytes <= address, "vertex buffer aliases the render target");
        GuestMemory::CheckRange(reinterpret_cast<const void*>(address), bytes, 1);
        auto buffer = std::make_unique<Buffer>(context, bytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        GuestMemory::Read(address, buffer->Bytes(), 1);
        vertexHandles.push_back(buffer->Handle());
        vertexBuffers.push_back(std::move(buffer));
    }
    timing.Mark("vertex_upload");
    RenderTarget* target = nullptr;
    if (state.hasColorTarget) {
        APS5_LOG_OUT_DEBUG("Creating color target address=0x%llx bytes=%llu extent=%ux%u", static_cast<unsigned long long>(state.color.address), static_cast<unsigned long long>(state.color.bytes), state.color.extent.width, state.color.extent.height);
        const ColorTargetLayout colorLayout(state.color.extent.width, state.color.extent.height, state.color.tileMode);
        Require(context.colorTransfer != nullptr, "device color transfer is unavailable");
        Require(state.color.bytes == colorLayout.Bytes(), "color target transfer size mismatch");
        context.colorTransfer->Upload(state.color.address, state.color.extent.width, state.color.extent.height, state.color.tileMode);
        timing.Mark("color_upload");
        APS5_LOG_CHARS_OUT_DEBUG("Color target read from guest memory");
        target = &context.colorTransfer->Target(state.color, state.blend.blendEnable != 0);
        timing.Mark("render_target_create");
        APS5_LOG_CHARS_OUT_DEBUG("RenderTarget created");
    }
    APS5_LOG_OUT_DEBUG("Color target object=%u", target ? 1u : 0u);
    APS5_LOG_CHARS_OUT_DEBUG("Creating ShaderResources");
    ShaderResources resources(context, shaders, state.color, draw.indexAddress, static_cast<std::size_t>(indexBytes), snapshots);
    timing.Mark("shader_resources");
    APS5_LOG_CHARS_OUT_DEBUG("ShaderResources created");
    APS5_LOG_CHARS_OUT_DEBUG("Creating Pipeline");
    Pipeline pipeline(context, state, target, resources, shaders);
    timing.Mark("pipeline_create");
    APS5_LOG_CHARS_OUT_DEBUG("Pipeline created");
    APS5_LOG_CHARS_OUT_DEBUG("Creating CommandBatch");
    CommandBatch batch(context);
    const auto commands = batch.Handle();
    APS5_LOG_CHARS_OUT_DEBUG("CommandBatch created");
    VkMemoryBarrier upload{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    upload.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    upload.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | shaderStages, 0, 1, &upload, 0, nullptr, 0, nullptr);
    APS5_LOG_CHARS_OUT_DEBUG("Upload barrier recorded");
    VkBufferImageCopy copy{};
    copy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copy.imageExtent = {state.color.extent.width, state.color.extent.height, 1};
    if (state.hasColorTarget) {
        APS5_LOG_CHARS_OUT_DEBUG("Copying guest color target to Vulkan image");
        context.colorTransfer->Detile(commands);
        imageBarrier(context, commands, target->Image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT);
        context.Function<PFN_vkCmdCopyBufferToImage>("vkCmdCopyBufferToImage")(commands, context.colorTransfer->LinearBuffer(), target->Image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
        imageBarrier(context, commands, target->Image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        APS5_LOG_CHARS_OUT_DEBUG("Guest color target upload recorded");
    }
    APS5_LOG_OUT_DEBUG("Beginning pipeline renderExtent=%ux%u", state.renderExtent.width, state.renderExtent.height);
    pipeline.Begin(commands, state.renderExtent);
    APS5_LOG_CHARS_OUT_DEBUG("Pipeline Begin OK");
    resources.Bind(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Layout());
    APS5_LOG_CHARS_OUT_DEBUG("Resources bound");
    pipeline.PushConstants(commands, shaders);
    APS5_LOG_CHARS_OUT_DEBUG("Push constants recorded");
    if (state.stages.mesh) {
        APS5_LOG_OUT_DEBUG("vkCmdDrawMeshTasksEXT groups=%u instances=%u", meshGroups, draw.instanceCount);
        context.Function<PFN_vkCmdDrawMeshTasksEXT>("vkCmdDrawMeshTasksEXT")(commands, meshGroups, draw.instanceCount, 1);
    } else {
        if (!vertexHandles.empty()) context.Function<PFN_vkCmdBindVertexBuffers>("vkCmdBindVertexBuffers")(commands, 0, static_cast<std::uint32_t>(vertexHandles.size()), vertexHandles.data(), vertexOffsets.data());
        if (draw.indexed) {
            context.Function<PFN_vkCmdBindIndexBuffer>("vkCmdBindIndexBuffer")(commands, indices->Handle(), 0, draw.indexSize == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
            context.Function<PFN_vkCmdDrawIndexed>("vkCmdDrawIndexed")(commands, draw.indexCount, draw.instanceCount, 0, 0, 0);
        } else {
            context.Function<PFN_vkCmdDraw>("vkCmdDraw")(commands, draw.indexCount, draw.instanceCount, draw.firstVertex, draw.firstInstance);
        }
    }
    APS5_LOG_CHARS_OUT_DEBUG("Draw recorded");
    context.Function<PFN_vkCmdEndRenderPass>("vkCmdEndRenderPass")(commands);
    APS5_LOG_CHARS_OUT_DEBUG("Render pass ended");
    if (state.hasColorTarget) {
        APS5_LOG_CHARS_OUT_DEBUG("Copying Vulkan image back to guest color target");
        imageBarrier(context, commands, target->Image(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);
        VkBufferMemoryBarrier reuse{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
        reuse.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        reuse.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        reuse.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        reuse.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        reuse.buffer = context.colorTransfer->LinearBuffer();
        reuse.size = VK_WHOLE_SIZE;
        context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &reuse, 0, nullptr);
        context.Function<PFN_vkCmdCopyImageToBuffer>("vkCmdCopyImageToBuffer")(commands, target->Image(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, context.colorTransfer->LinearBuffer(), 1, &copy);
        context.colorTransfer->Tile(commands);
        APS5_LOG_CHARS_OUT_DEBUG("Color target copy-back recorded");
    }
    VkMemoryBarrier download{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    download.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    download.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    context.Function<PFN_vkCmdPipelineBarrier>("vkCmdPipelineBarrier")(commands, VK_PIPELINE_STAGE_TRANSFER_BIT | shaderStages, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &download, 0, nullptr, 0, nullptr);
    APS5_LOG_CHARS_OUT_DEBUG("Download barrier recorded");
    APS5_LOG_CHARS_OUT_DEBUG("SubmitAndWait");
    timing.Mark("command_record");
    batch.SubmitAndWait();
    timing.Mark("submit_wait");
    APS5_LOG_CHARS_OUT_DEBUG("SubmitAndWait OK");
    if (state.hasColorTarget) GuestMemory::CheckRange(reinterpret_cast<const void*>(state.color.address), state.color.bytes, 256, true);
    if (state.hasColorTarget) APS5_LOG_CHARS_OUT_DEBUG("Color target guest range OK");
    timing.Mark("color_range_check");
    APS5_LOG_CHARS_OUT_DEBUG("Shader resources WriteBack");
    resources.WriteBack();
    timing.Mark("resources_writeback");
    APS5_LOG_CHARS_OUT_DEBUG("Shader resources WriteBack OK");
    if (state.hasColorTarget) context.colorTransfer->WriteBack(state.color.address);
    timing.Mark("color_writeback");
    if (state.hasColorTarget) APS5_LOG_OUT_DEBUG("Color target written to guest address=0x%llx bytes=%llu", static_cast<unsigned long long>(state.color.address), static_cast<unsigned long long>(state.color.bytes));
    APS5_LOG_CHARS_OUT("Draw finished");
}

}
