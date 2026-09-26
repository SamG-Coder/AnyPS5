#include "prx/libSceAgcDriver/Graphics/include/DepthSurface.hpp"
#include "prx/libSceAgcDriver/Execution/include/GuestMemory.hpp"
#include "prx/libc/include/GuestMemoryBacking.hpp"
#include <SDL_loadso.h>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace AgcDriver::Graphics;
class Device {
public:
    Device() {
#ifdef _WIN32
        library = SDL_LoadObject("vulkan-1.dll");
#else
        library = SDL_LoadObject("libvulkan.so.1");
#endif
        Require(library != nullptr, "cannot load Vulkan");
        try {
            instanceProc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_LoadFunction(library, "vkGetInstanceProcAddr"));
            Require(instanceProc != nullptr, "missing Vulkan instance resolver");
            VkApplicationInfo application{VK_STRUCTURE_TYPE_APPLICATION_INFO};
            application.apiVersion = VK_API_VERSION_1_1;
            VkInstanceCreateInfo info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
            info.pApplicationInfo = &application;
            Check(function<PFN_vkCreateInstance>("vkCreateInstance")(&info, nullptr, &instance), "vkCreateInstance");
            std::uint32_t count = 0;
            const auto enumerate = function<PFN_vkEnumeratePhysicalDevices>("vkEnumeratePhysicalDevices");
            Check(enumerate(instance, &count, nullptr), "vkEnumeratePhysicalDevices");
            Require(count != 0, "no Vulkan device");
            std::vector<VkPhysicalDevice> devices(count);
            Check(enumerate(instance, &count, devices.data()), "vkEnumeratePhysicalDevices");
            context.physical = devices.front();
            const auto queues = function<PFN_vkGetPhysicalDeviceQueueFamilyProperties>("vkGetPhysicalDeviceQueueFamilyProperties");
            queues(context.physical, &count, nullptr);
            std::vector<VkQueueFamilyProperties> families(count);
            queues(context.physical, &count, families.data());
            std::uint32_t family = 0;
            while (family < count && (families[family].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) ++family;
            Require(family < count, "no Vulkan graphics queue");
            const float priority = 1;
            VkDeviceQueueCreateInfo queue{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
            queue.queueFamilyIndex = family;
            queue.queueCount = 1;
            queue.pQueuePriorities = &priority;
            VkDeviceCreateInfo device{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
            device.queueCreateInfoCount = 1;
            device.pQueueCreateInfos = &queue;
            Check(function<PFN_vkCreateDevice>("vkCreateDevice")(context.physical, &device, nullptr, &context.device), "vkCreateDevice");
            context.deviceProc = function<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");
            function<PFN_vkGetPhysicalDeviceMemoryProperties>("vkGetPhysicalDeviceMemoryProperties")(context.physical, &context.memory);
            VkPhysicalDeviceProperties properties{};
            function<PFN_vkGetPhysicalDeviceProperties>("vkGetPhysicalDeviceProperties")(context.physical, &properties);
            context.limits = properties.limits;
            context.formatProperties = function<PFN_vkGetPhysicalDeviceFormatProperties>("vkGetPhysicalDeviceFormatProperties");
            context.imageFormatProperties = function<PFN_vkGetPhysicalDeviceImageFormatProperties>("vkGetPhysicalDeviceImageFormatProperties");
            context.Function<PFN_vkGetDeviceQueue>("vkGetDeviceQueue")(context.device, family, 0, &context.queue);
            VkCommandPoolCreateInfo pool{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
            pool.queueFamilyIndex = family;
            Check(context.Function<PFN_vkCreateCommandPool>("vkCreateCommandPool")(context.device, &pool, nullptr, &context.pool), "vkCreateCommandPool");
        } catch (...) {
            release();
            throw;
        }
    }

    ~Device() { release(); }
    const Context& GetContext() const { return context; }

private:
    template<typename TFunction>
    TFunction function(const char* name) const {
        const auto result = reinterpret_cast<TFunction>(instanceProc(instance, name));
        Require(result != nullptr, name);
        return result;
    }

    void release() noexcept {
        if (context.pool != VK_NULL_HANDLE) context.Function<PFN_vkDestroyCommandPool>("vkDestroyCommandPool")(context.device, context.pool, nullptr);
        context.bufferPool.reset();
        if (context.device != VK_NULL_HANDLE) function<PFN_vkDestroyDevice>("vkDestroyDevice")(context.device, nullptr);
        if (instance != VK_NULL_HANDLE) function<PFN_vkDestroyInstance>("vkDestroyInstance")(instance, nullptr);
        if (library != nullptr) SDL_UnloadObject(library);
    }

    void* library = nullptr;
    PFN_vkGetInstanceProcAddr instanceProc = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    Context context{};
};

VkShaderModule LoadShader(const Context& context, const char* path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    Require(file.is_open(), "missing depth test shader");
    const auto size = static_cast<std::size_t>(file.tellg());
    Require(size >= 20 && size % 4 == 0, "invalid depth test shader");
    std::vector<std::uint32_t> words(size / 4);
    file.seekg(0);
    Require(static_cast<bool>(file.read(reinterpret_cast<char*>(words.data()), size)), "shader read failed");
    VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    info.codeSize = size;
    info.pCode = words.data();
    VkShaderModule shader{};
    Check(context.Function<PFN_vkCreateShaderModule>("vkCreateShaderModule")(context.device, &info, nullptr, &shader), "create depth test shader");
    return shader;
}

void TestDepth(const Context& context, const char* vertexPath, const char* fragmentPath, bool resident = false) {
    constexpr VkExtent2D extent{129, 131};
    DepthTargetLayout layout(extent.width, extent.height);
    const DepthState state{0, extent, layout.Bytes(), VK_COMPARE_OP_LESS, true};
    DepthSurface surface(context, state);
    std::vector<std::byte> tiled(layout.Bytes(), std::byte{0xa5});
    std::vector<float> linear(extent.width * extent.height, 1.0f);
    layout.Tile(std::as_bytes(std::span(linear)), tiled);
    std::shared_ptr<void> guest;
    std::uint64_t guestAddress = 0;
    if (resident) {
        guest = std::shared_ptr<void>(GuestMemoryBacking::GuestMemoryBackingMap_nid_postfix(nullptr, layout.Bytes(), 65536, 3),
            [bytes = layout.Bytes()](void* memory) { GuestMemoryBacking::GuestMemoryBackingUnmap_nid_postfix(memory, bytes); });
        guestAddress = reinterpret_cast<std::uint64_t>(guest.get());
        AgcDriver::GuestMemory::Write(guestAddress, tiled, 65536);
        surface.BindGuest(guestAddress, [](bool) {});
    }
    const auto vertex = LoadShader(context, vertexPath);
    const auto fragment = LoadShader(context, fragmentPath);
    const VkPushConstantRange push{VK_SHADER_STAGE_VERTEX_BIT, 0, 4};
    VkPipelineLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges = &push;
    VkPipelineLayout pipelineLayout{};
    Check(context.Function<PFN_vkCreatePipelineLayout>("vkCreatePipelineLayout")(context.device, &layoutInfo, nullptr, &pipelineLayout), "depth test layout");
    VkAttachmentDescription attachment{};
    attachment.format = VK_FORMAT_D32_SFLOAT;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.initialLayout = attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    const VkAttachmentReference reference{0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pDepthStencilAttachment = &reference;
    VkRenderPassCreateInfo passInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    passInfo.attachmentCount = 1;
    passInfo.pAttachments = &attachment;
    passInfo.subpassCount = 1;
    passInfo.pSubpasses = &subpass;
    VkRenderPass pass{};
    Check(context.Function<PFN_vkCreateRenderPass>("vkCreateRenderPass")(context.device, &passInfo, nullptr, &pass), "depth test pass");
    const auto view = surface.View();
    VkFramebufferCreateInfo framebufferInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    framebufferInfo.renderPass = pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &view;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;
    VkFramebuffer framebuffer{};
    Check(context.Function<PFN_vkCreateFramebuffer>("vkCreateFramebuffer")(context.device, &framebufferInfo, nullptr, &framebuffer), "depth test framebuffer");
    std::array<VkPipelineShaderStageCreateInfo, 2> stages{};
    for (auto& stage : stages) { stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; stage.pName = "main"; }
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertex;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fragment;
    VkPipelineVertexInputStateCreateInfo input{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    VkPipelineInputAssemblyStateCreateInfo assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkViewport viewport{0, 0, static_cast<float>(extent.width), static_cast<float>(extent.height), 0, 1};
    VkRect2D scissor{{0, 0}, extent};
    VkPipelineViewportStateCreateInfo viewportInfo{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportInfo.viewportCount = viewportInfo.scissorCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.pScissors = &scissor;
    VkPipelineRasterizationStateCreateInfo raster{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster.lineWidth = 1;
    VkPipelineMultisampleStateCreateInfo samples{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    samples.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineDepthStencilStateCreateInfo depth{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth.depthTestEnable = VK_TRUE;
    VkPipelineColorBlendStateCreateInfo blend{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    VkGraphicsPipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages.data();
    pipelineInfo.pVertexInputState = &input;
    pipelineInfo.pInputAssemblyState = &assembly;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &raster;
    pipelineInfo.pMultisampleState = &samples;
    pipelineInfo.pDepthStencilState = &depth;
    pipelineInfo.pColorBlendState = &blend;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = pass;
    const auto draw = [&](float value, VkCompareOp comparison, bool write, float expected, bool readback = true) {
        depth.depthCompareOp = comparison;
        depth.depthWriteEnable = write;
        VkPipeline pipeline{};
        Check(context.Function<PFN_vkCreateGraphicsPipelines>("vkCreateGraphicsPipelines")(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "depth test pipeline");
        CommandBatch batch(context);
        const auto commands = batch.Handle();
        if (resident) surface.BeginGuest(commands, write);
        else surface.Upload(commands, tiled);
        VkRenderPassBeginInfo begin{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        begin.renderPass = pass;
        begin.framebuffer = framebuffer;
        begin.renderArea = {{0, 0}, extent};
        context.Function<PFN_vkCmdBeginRenderPass>("vkCmdBeginRenderPass")(commands, &begin, VK_SUBPASS_CONTENTS_INLINE);
        context.Function<PFN_vkCmdBindPipeline>("vkCmdBindPipeline")(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        context.Function<PFN_vkCmdPushConstants>("vkCmdPushConstants")(commands, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, 4, &value);
        context.Function<PFN_vkCmdDraw>("vkCmdDraw")(commands, 3, 1, 0, 0);
        context.Function<PFN_vkCmdEndRenderPass>("vkCmdEndRenderPass")(commands);
        if (!resident) surface.Download(commands);
        batch.SubmitAndWait();
        if (readback) {
            if (resident) AgcDriver::GuestMemory::Read(guestAddress, tiled, 65536);
            else surface.Read(tiled);
            layout.Detile(tiled, std::as_writable_bytes(std::span(linear)));
            for (const auto pixel : linear) Require(pixel == expected, "GPU depth comparison or persistence failed");
            Require(tiled.back() == std::byte{0xa5}, "depth transfer changed tile padding");
        }
        context.Function<PFN_vkDestroyPipeline>("vkDestroyPipeline")(context.device, pipeline, nullptr);
    };
    draw(0.75f, VK_COMPARE_OP_LESS, true, 0.75f);
    draw(0.25f, VK_COMPARE_OP_LESS, true, 0.25f);
    draw(0.75f, VK_COMPARE_OP_LESS, true, 0.25f);
    draw(0.125f, VK_COMPARE_OP_LESS, false, 0.25f);
    draw(0.75f, VK_COMPARE_OP_ALWAYS, true, 0.75f);
    draw(0.25f, VK_COMPARE_OP_GREATER, true, 0.75f);
    std::fill(linear.begin(), linear.end(), 1.0f);
    layout.Tile(std::as_bytes(std::span(linear)), tiled);
    if (resident) AgcDriver::GuestMemory::Write(guestAddress, tiled, 65536);
    draw(0.5f, VK_COMPARE_OP_LESS, true, 0.5f);
    if (resident) {
        draw(0.25f, VK_COMPARE_OP_LESS, true, 0.25f, false);
        draw(0.75f, VK_COMPARE_OP_LESS, true, 0.25f);
        draw(0.125f, VK_COMPARE_OP_LESS, true, 0.125f, false);
        Require(*reinterpret_cast<volatile float*>(guestAddress) == 0.125f, "native CPU read missed GPU depth writes");
        std::fill(linear.begin(), linear.end(), 1.0f);
        layout.Tile(std::as_bytes(std::span(linear)), tiled);
        std::memcpy(reinterpret_cast<void*>(guestAddress), tiled.data(), tiled.size());
        draw(0.5f, VK_COMPARE_OP_LESS, true, 0.5f);
        draw(0.125f, VK_COMPARE_OP_LESS, true, 0.125f, false);
        surface.ReleaseGuest();
        AgcDriver::GuestMemory::Read(guestAddress, tiled, 65536);
        layout.Detile(tiled, std::as_writable_bytes(std::span(linear)));
        for (const auto pixel : linear) Require(pixel == 0.125f, "releasing depth ownership lost GPU writes");
    }
    context.Function<PFN_vkDestroyFramebuffer>("vkDestroyFramebuffer")(context.device, framebuffer, nullptr);
    context.Function<PFN_vkDestroyRenderPass>("vkDestroyRenderPass")(context.device, pass, nullptr);
    context.Function<PFN_vkDestroyPipelineLayout>("vkDestroyPipelineLayout")(context.device, pipelineLayout, nullptr);
    context.Function<PFN_vkDestroyShaderModule>("vkDestroyShaderModule")(context.device, vertex, nullptr);
    context.Function<PFN_vkDestroyShaderModule>("vkDestroyShaderModule")(context.device, fragment, nullptr);
}

static void TestReadback(const Context& context) {
    const VkExtent2D extent{3840, 2160};
    const DepthTargetLayout layout(extent.width, extent.height);
    DepthState state{};
    state.extent = extent;
    state.bytes = layout.Bytes();
    DepthSurface surface(context, state);
    std::vector<float> linear(static_cast<std::size_t>(extent.width) * extent.height);
    for (std::size_t i = 0; i < linear.size(); ++i) linear[i] = static_cast<float>(i % 1024) / 1024.0f;
    std::vector<std::byte> tiled(layout.Bytes(), std::byte{0xa5});
    layout.Tile(std::as_bytes(std::span(linear)), tiled);
    const auto expected = tiled;
    for (unsigned iteration = 0; iteration < 3; ++iteration) {
        CommandBatch batch(context);
        surface.Upload(batch.Handle(), tiled);
        surface.Download(batch.Handle());
        batch.SubmitAndWait();
        const auto start = std::chrono::steady_clock::now();
        surface.Read(tiled);
        const auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        Require(tiled == expected, "4K depth readback changed pixels or padding");
        std::cout << "4K depth readback: " << elapsed << " ms\n";
    }
}

int main(int argc, char** argv) {
    try {
        Require(argc == 3, "expected depth vertex and fragment SPIR-V paths");
        Device device;
        TestDepth(device.GetContext(), argv[1], argv[2]);
        TestDepth(device.GetContext(), argv[1], argv[2], true);
        TestReadback(device.GetContext());
        std::cout << "Vulkan D32 occlusion, write masks, CPU clears and tiled readback passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
