#include "prx/libSceAgcDriver/Execution/include/NativeCompletionQueue.hpp"
#include <atomic>
#include <cstdlib>
#include <future>
#include <stdexcept>
#include <vector>

static void Require(bool value) {
    if (!value) std::abort();
}

int main() {
    AgcDriver::NativeCompletionQueue queue;
    std::atomic<bool> ready{false};
    std::atomic<bool> polled{false};
    std::atomic<unsigned> completed{0};
    std::promise<void> firstPoll;
    std::vector<unsigned> order;
    const auto fail = [](std::exception_ptr) { std::abort(); };
    queue.Enqueue([&] {
        if (!polled.exchange(true)) firstPoll.set_value();
        return ready.load();
    }, [&] { order.push_back(1); ++completed; }, fail);
    queue.Enqueue([] { return true; }, [&] { order.push_back(2); ++completed; }, fail);
    Require(firstPoll.get_future().wait_for(std::chrono::seconds(2)) == std::future_status::ready);
    Require(completed == 0);
    ready = true;
    queue.WaitIdle();
    Require(completed == 2 && order == std::vector<unsigned>{1, 2});

    std::exception_ptr captured;
    queue.Enqueue([]() -> bool { throw std::runtime_error("fence failure"); },
        [] { std::abort(); }, [&](std::exception_ptr error) { captured = error; });
    queue.WaitIdle();
    Require(captured != nullptr);
    try {
        std::rethrow_exception(captured);
    } catch (const std::runtime_error& error) {
        Require(std::string(error.what()) == "fence failure");
    }

    queue.Enqueue([] { return true; }, [&] {
        queue.Enqueue([] { return true; }, [&] { ++completed; }, fail);
    }, fail);
    queue.WaitIdle();
    Require(completed == 3);

    captured = nullptr;
    queue.Enqueue([] { return true; }, [] { throw std::runtime_error("completion failure"); },
        [&](std::exception_ptr error) { captured = error; });
    queue.WaitIdle();
    Require(captured != nullptr);

    AgcDriver::NativeCompletionQueue pending;
    pending.Enqueue([] { return false; }, [] { std::abort(); }, fail);
}
