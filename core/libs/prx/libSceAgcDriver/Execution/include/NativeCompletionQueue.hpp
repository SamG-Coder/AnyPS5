#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVECOMPLETIONQUEUE_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_NATIVECOMPLETIONQUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>

namespace AgcDriver {
class NativeCompletionQueue {
public:
    ~NativeCompletionQueue() { Stop(); }
    void Stop() {
        {
            std::lock_guard lock(mutex);
            stopping = true;
        }
        changed.notify_all();
        if (worker.joinable()) worker.join();
    }
    void Enqueue(std::function<bool()> ready, std::function<void()> complete,
                 std::function<void(std::exception_ptr)> fail) {
        auto task = std::make_shared<Task>(Task{std::move(ready), std::move(complete), std::move(fail)});
        {
            std::lock_guard lock(mutex);
            if (stopping) throw std::runtime_error("native completion queue is stopped");
            if (!worker.joinable()) worker = std::thread([this] { Run(); });
            tasks.push_back(std::move(task));
        }
        changed.notify_all();
    }
    void WaitIdle() {
        std::unique_lock lock(mutex);
        changed.wait(lock, [this] { return tasks.empty(); });
    }

private:
    struct Task {
        std::function<bool()> ready;
        std::function<void()> complete;
        std::function<void(std::exception_ptr)> fail;
    };
    void Run() {
        for (;;) {
            std::shared_ptr<Task> task;
            {
                std::unique_lock lock(mutex);
                changed.wait(lock, [this] { return stopping || !tasks.empty(); });
                if (stopping) return;
                task = tasks.front();
            }
            bool finished = false;
            try {
                if (task->ready()) {
                    task->complete();
                    finished = true;
                }
            } catch (...) {
                task->fail(std::current_exception());
                finished = true;
            }
            std::unique_lock lock(mutex);
            if (finished) {
                tasks.pop_front();
                changed.notify_all();
            } else {
                changed.wait_for(lock, std::chrono::milliseconds(1), [this] { return stopping; });
            }
        }
    }
    std::mutex mutex;
    std::condition_variable changed;
    std::deque<std::shared_ptr<Task>> tasks;
    bool stopping = false;
    std::thread worker;
};
}

#endif
