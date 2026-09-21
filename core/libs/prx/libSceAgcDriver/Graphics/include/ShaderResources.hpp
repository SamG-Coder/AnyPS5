#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_SHADERRESOURCES_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_SHADERRESOURCES_HPP

#include "prx/libSceAgcDriver/Graphics/include/Resources.hpp"
#include "Recompiler.hpp"
#include "prx/libSceAgcDriver/Graphics/include/Shaders.hpp"
#include <array>
#include <memory>
#include <vector>

namespace AgcDriver::Graphics {

class ShaderResources {
public:
    ShaderResources(const Context& context, const ShaderRecompiler::RecompileResult& vertex, const ShaderRecompiler::RecompileResult& fragment, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes);
    ShaderResources(const Context& context, std::span<const CompiledShader> shaders, const ColorTarget& target, std::uint64_t indexAddress, std::size_t indexBytes);
    ShaderResources(const Context& context, const CompiledShader& compute);
    ~ShaderResources();
    ShaderResources(const ShaderResources&) = delete;
    ShaderResources& operator=(const ShaderResources&) = delete;
    VkDescriptorSetLayout Layout() const;
    void Bind(VkCommandBuffer commands, VkPipelineBindPoint bindPoint, VkPipelineLayout layout) const;
    void WriteBack();

private:
    struct Allocation {
        std::uint64_t address;
        std::size_t size;
        bool guest;
        std::unique_ptr<Buffer> buffer;
    };

    void build(std::span<const CompiledShader> shaders, const ColorTarget* target, std::uint64_t indexAddress, std::size_t indexBytes);
    std::size_t addGuestBuffer(std::span<const std::uint32_t> words, const ColorTarget* target, std::uint64_t indexAddress, std::size_t indexBytes);
    std::size_t addDataBuffer(std::span<const std::uint32_t> words);
    void release() noexcept;
    const Context& context;
    VkDescriptorSetLayout _layout = VK_NULL_HANDLE;
    VkDescriptorSet _set = VK_NULL_HANDLE;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    std::vector<Allocation> allocations;
};

}

#endif
