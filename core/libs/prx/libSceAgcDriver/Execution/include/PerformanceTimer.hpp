#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PERFORMANCETIMER_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_PERFORMANCETIMER_HPP

#include "prx/libc/include/general/LogMacros.hpp"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <stdexcept>

namespace AgcDriver {

class PerformanceTimer {
public:
    explicit PerformanceTimer(const char* scope) : scope(scope) {}
    PerformanceTimer(const PerformanceTimer&) = delete;
    PerformanceTimer& operator=(const PerformanceTimer&) = delete;

    ~PerformanceTimer() {
        const auto end = Clock::now();
        APS5_LOG_OUT("[Timing] id=%llu scope=%s total_ms=%.3f tail_ms=%.3f unwinding=%u%s", id, scope, milliseconds(end - start), milliseconds(end - previous), std::uncaught_exceptions() > exceptions ? 1u : 0u, stages);
    }

    void Mark(const char* stage) {
        const auto now = Clock::now();
        const auto count = std::snprintf(stages + used, sizeof(stages) - used, " %s_ms=%.3f", stage, milliseconds(now - previous));
        if (count < 0 || static_cast<std::size_t>(count) >= sizeof(stages) - used) {
            throw std::runtime_error("PerformanceTimer: timing log capacity exceeded");
        }
        used += static_cast<std::size_t>(count);
        previous = now;
    }

private:
    using Clock = std::chrono::steady_clock;

    static double milliseconds(Clock::duration duration) {
        return std::chrono::duration<double, std::milli>(duration).count();
    }

    inline static std::atomic<unsigned long long> nextId{0};
    const char* scope;
    unsigned long long id = nextId.fetch_add(1, std::memory_order_relaxed);
    Clock::time_point start = Clock::now();
    Clock::time_point previous = start;
    int exceptions = std::uncaught_exceptions();
    char stages[2048]{};
    std::size_t used = 0;
};

}

#endif
