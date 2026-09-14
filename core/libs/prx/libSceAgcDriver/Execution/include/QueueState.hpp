#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_QUEUESTATE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_QUEUESTATE_HPP

#include <cstdint>
#include <map>

namespace AgcDriver {

using Registers = std::map<std::uint32_t, std::uint32_t>;

struct QueueState {
    Registers shader;
    Registers context;
    Registers userConfig;

    void ClearContext() {
        context.clear();
    }
};

}

#endif
