#pragma once
#include "SignalMask.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstddef>

namespace GuestSignals {
using Handler = void (APS5_VABI *)(int);
struct Action {
    Handler handler;
    std::int32_t flags;
    Mask mask;
};
static_assert(offsetof(Action, flags) == 8);
static_assert(offsetof(Action, mask) == 12);
static_assert(sizeof(Action) == 32);
}
