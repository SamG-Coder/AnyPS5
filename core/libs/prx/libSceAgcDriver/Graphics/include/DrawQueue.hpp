#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DRAWQUEUE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_GRAPHICS_INCLUDE_DRAWQUEUE_HPP

#include "prx/libSceAgcDriver/Graphics/include/ShaderResources.hpp"

namespace AgcDriver::Graphics {

class DrawQueue {
public:
    ~DrawQueue();
    void Submit(std::unique_ptr<CommandBatch> commands, std::shared_ptr<ShaderResources> resources, std::shared_ptr<void> storage);
    void Resolve(std::uint64_t address, std::size_t bytes);
    void Wait();

private:
    struct Entry {
        std::shared_ptr<void> storage;
        std::shared_ptr<ShaderResources> resources;
        std::unique_ptr<CommandBatch> commands;
    };
    std::vector<Entry> pending;
};

}

#endif
