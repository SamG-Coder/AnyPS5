#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_QUEUESTATE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_QUEUESTATE_HPP

#include <cstdint>
#include <map>
#include <array>
#include <optional>
#include <string>
#include <vector>

namespace AgcDriver {

using Registers = std::map<std::uint32_t, std::uint32_t>;

struct QueueState {
    Registers shader;
    Registers context;
    Registers userConfig;
    std::optional<Registers> savedContext;
    std::array<std::uint32_t, 0x3000> constantRam{};
    std::uint64_t indexBase = 0;
    std::uint64_t drawIndirectBase = 0;
    std::uint64_t dispatchIndirectBase = 0;
    std::uint32_t indexBufferSize = 0;
    std::uint32_t indexType = 0;
    std::uint32_t instanceCount = 1;
    std::vector<std::string> markers;

    void ClearContext() {
        context.clear();
    }
};

}

#endif
