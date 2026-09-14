#include "prx/libSceVideoOut/include/VideoOutDriver.hpp"
#include "prx/libSceVideoOut/include/Buffer.hpp"
#include "prx/libSceVideoOut/include/Output.hpp"
#include "prx/libSceAgcDriver/Execution/include/Driver.hpp"
#include "prx/libSceAgcDriver/Submit/include/Dcb.hpp"
#include "prx/libc/include/Shutdown.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <string>

#undef main

namespace {

void check(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(reason);
}

template<typename TAction>
std::string expectFailure(TAction action) {
    try { action(); }
    catch (const std::runtime_error& error) { return error.what(); }
    throw std::runtime_error("expected an exception");
}

class Gate final : public AgcDriver::IVideoOutput, public AgcDriver::IFlipRequest, public std::enable_shared_from_this<Gate> {
public:
    std::mutex mutex;
    std::condition_variable changed;
    bool released = false;
    std::shared_ptr<AgcDriver::IFlipRequest> Reserve(const AgcDriver::FlipInfo&) override { return shared_from_this(); }
    void GpuReady() override {
        std::unique_lock lock(mutex);
        if (!changed.wait_for(lock, std::chrono::seconds(10), [&] { return released; })) throw std::runtime_error("test gate timed out");
    }
    void Fail(std::exception_ptr error) noexcept override { if (!error) std::terminate(); }
    void Release() {
        std::lock_guard lock(mutex);
        released = true;
        changed.notify_all();
    }
};

void testLifetime(bool reopen) {
    const int handle = sceVideoOutOpen(255, 0, 0, nullptr);
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    auto gate = std::make_shared<Gate>();
    AgcDriverRegisterVideoOutput_nid_postfix(7, gate);
    std::array<uint32_t, 6> words{0xc004105c, 7, 0xfffffffeu, 1, 0, 0};
    Packet packet{words.data(), 6, 0, {}};
    sceAgcDriverSubmitDcb(&packet);
    alignas(65536) static std::array<std::byte, 65536> storage{};
    VideoOutBuffers buffer{storage.data(), nullptr, {nullptr, nullptr}};
    VideoOutBufferAttribute2 attribute{};
    attribute.width = 64;
    attribute.height = 64;
    sceVideoOutRegisterBuffers2(handle, 0, 0, &buffer, 1, &attribute, 0, nullptr);
    sceVideoOutSubmitFlip(handle, 0, 1, -9);
    {
        std::lock_guard lock(cfg->mutex);
        check(cfg->flipStatus.flipPendingNum == 1 && cfg->flipStatus.count == 0, "reservation status is wrong");
    }
    attribute.dcc_control = 1;
    sceVideoOutSubmitChangeBufferAttribute2(handle, 0, &attribute, nullptr);
    sceVideoOutUnregisterBuffers(handle, 0);
    std::shared_ptr<VideoOutConfig> replacement;
    if (reopen) {
        sceVideoOutClose(handle);
        check(sceVideoOutOpen(255, 0, 0, nullptr) == handle, "reopen changed handle");
        replacement = VideoOutDriver::Get().GetConfig(handle);
        check(replacement != cfg && replacement->generation > cfg->generation, "reopen reused old port state");
    }
    gate->Release();
    std::exception_ptr failure;
    {
        std::unique_lock lock(cfg->mutex);
        check(cfg->vblankCond.wait_for(lock, std::chrono::seconds(10), [&] { return cfg->failure != nullptr; }), "flip failure did not wake waiters");
        failure = cfg->failure;
        check(cfg->flipStatus.count == 0 && cfg->flipStatus.flipPendingNum == 0, "failed flip has successful or pending status");
    }
    const auto message = expectFailure([&] { std::rethrow_exception(failure); });
    check(message.find(reopen ? "closed" : "tiling") != std::string::npos, "flip used changed attributes or port generation");
    if (replacement) {
        std::lock_guard lock(replacement->mutex);
        check(replacement->flipStatus.flipPendingNum == 0 && replacement->flipStatus.count == 0, "old request changed new port counters");
    }
    check(expectFailure([&] { sceVideoOutWaitVblank(handle); }).find(reopen ? "closed" : "tiling") != std::string::npos, "VideoOut lost worker failure");
    AgcDriverUnregisterVideoOutput_nid_postfix(7, gate);
    sceVideoOutClose(handle);
    const auto shutdown = expectFailure([] { LibcRunShutdown_nid_postfix(); });
    check(shutdown.find(reopen ? "closed" : "tiling") != std::string::npos, "shutdown lost asynchronous error");
}

void testPresentation() {
    const int handle = sceVideoOutOpen(255, 0, 0, nullptr);
    auto cfg = VideoOutDriver::Get().GetConfig(handle);
    {
        std::lock_guard lock(cfg->mutex);
        cfg->width = 64;
        cfg->height = 64;
    }
    for (int index : {-2, -1, -2}) {
        uint64_t target;
        {
            std::lock_guard lock(cfg->mutex);
            target = cfg->flipStatus.count + 1;
        }
        sceVideoOutSubmitFlip(handle, index, 1, -123456789);
        std::unique_lock lock(cfg->mutex);
        check(cfg->vblankCond.wait_for(lock, std::chrono::seconds(15), [&] { return cfg->failure || cfg->flipStatus.count == target; }), "presentation did not complete");
        if (cfg->failure) std::rethrow_exception(cfg->failure);
        check(cfg->flipStatus.flipArg == -123456789 && cfg->flipStatus.currentBuffer == index && cfg->flipStatus.flipPendingNum == 0, "presentation status is wrong");
    }
    sceVideoOutClose(handle);
    LibcRunShutdown_nid_postfix();
}

}

int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::string(argv[1]) == "present") testPresentation();
        else testLifetime(argc == 2 && std::string(argv[1]) == "reopen");
        std::puts("VideoOut flip tests passed");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        try { LibcRunShutdown_nid_postfix(); }
        catch (const std::exception& shutdown) { std::fprintf(stderr, "shutdown: %s\n", shutdown.what()); }
        return 1;
    }
}
