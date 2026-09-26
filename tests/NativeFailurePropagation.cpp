#include "prx/libSceAgcDriver/Execution/include/NativeAgc.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Execution/include/Presentation.hpp"
#include "prx/libSceAgcDriver/Execution/include/VideoOutput.hpp"
#include <atomic>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

extern "C" int APS5_VABI aps5NativeAgcSuspendPoint();

namespace {
std::string message(std::exception_ptr error) {
    try { std::rethrow_exception(error); }
    catch (const std::runtime_error& e) { return e.what(); }
}
class Output final : public AgcDriver::IVideoOutput, public std::enable_shared_from_this<Output> {
public:
    explicit Output(std::uint32_t handle) : handle(handle) {}
    std::shared_ptr<AgcDriver::IFlipRequest> Reserve(const AgcDriver::FlipInfo&) override {
        throw std::runtime_error("unexpected reservation");
    }
    void Fail(std::exception_ptr error) noexcept override {
        try {
            if (message(error) != "first failure") failed = true;
            // Re-entry proves notifications are outside the registry mutex.
            AgcDriverUnregisterVideoOutput_nid_postfix(handle, shared_from_this());
            ++calls;
        } catch (...) { failed = true; }
    }
    std::uint32_t handle;
    std::atomic<unsigned> calls{0};
    std::atomic<bool> failed{false};
};
template<class F> void rejectsFirst(F action) {
    try { action(); } catch (const std::runtime_error& error) {
        if (std::string(error.what()) == "first failure") return;
        throw;
    }
    throw std::runtime_error("asynchronous failure was not propagated");
}
}
int main() {
    std::array<std::uint32_t, 8> words{};
    CommandBuffer commands{words.data(), words.data() + words.size(), words.data(),
        words.data() + words.size(), nullptr, nullptr, 0};
    Label marker{17};
    aps5NativeAgcReleaseMem(&commands, 0x28, 0x30c, 0, 0, &marker, 1, 42, 0, 0, 0, 0);
    const Packet pending{words.data(), 8, 0, {0, 0, 0}};
    const auto a = std::make_shared<Output>(1), b = std::make_shared<Output>(2);
    AgcDriverRegisterVideoOutput_nid_postfix(1, a);
    AgcDriverRegisterVideoOutput_nid_postfix(2, b);
    const auto first = std::make_exception_ptr(std::runtime_error("first failure"));
    std::atomic<bool> reporterThrew{false};
    std::thread reporter([&] {
        try { AgcDriverReportFailure_nid_postfix(first); }
        catch (...) { reporterThrew = true; }
    });
    reporter.join();
    if (reporterThrew || a->calls != 1 || b->calls != 1 || a->failed || b->failed) return 1;
    std::vector<std::thread> repeats;
    for (unsigned i = 0; i < 8; ++i) repeats.emplace_back([] {
        AgcDriverReportFailure_nid_postfix(std::make_exception_ptr(std::runtime_error("later failure")));
    });
    for (auto& thread : repeats) thread.join();
    rejectsFirst([] { AgcDriverWaitIdle_nid_postfix(); });
    rejectsFirst([] { AgcDriverSuspendPoint_nid_postfix(); });
    rejectsFirst([] { aps5NativeAgcSuspendPoint(); });
    rejectsFirst([] { aps5NativeAgcSubmit(nullptr); });
    rejectsFirst([&] { aps5NativeAgcSubmit(&pending); });
    if (marker.value != 17) return 3;
    rejectsFirst([&] { AgcDriverRegisterVideoOutput_nid_postfix(3, a); });
    AgcDriverReleaseWindow_nid_postfix(nullptr);
    return a->calls == 1 && b->calls == 1 ? 0 : 2;
}
