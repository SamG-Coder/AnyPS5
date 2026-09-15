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
    ~ShaderResources();
    ShaderResources(const ShaderResources&) = delete;
    ShaderResources& operator=(const ShaderResources&) = delete;
    const std::vector<VkDescriptorSetLayout>& Layouts() const;
    void Bind(VkCommandBuffer commands, VkPipelineLayout layout) const;
    void WriteBack();

private:
    struct Allocation {
        std::uint64_t address;
        std::size_t size;
        bool writable;
        std::unique_ptr<Buffer> buffer;
    };

    void release() noexcept;
    const Context& context;
    std::vector<VkDescriptorSetLayout> _layouts;
    std::vector<VkDescriptorSet> _sets;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    std::vector<Allocation> allocations;
};

}

#endif
