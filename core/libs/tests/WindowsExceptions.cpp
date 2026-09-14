#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <array>
#include <exception>
#include <thread>

extern "C" void NotImplemented_nid_no_patch(const char*);

static int destroyed;
struct Guard { ~Guard() { ++destroyed; } };
static void ThrowNested() {
    Guard guard;
    throw std::runtime_error("native own unwind");
}
static void Rethrow() {
    Guard guard;
    try { ThrowNested(); }
    catch (const std::runtime_error&) { throw; }
}

class TrackedError : public std::runtime_error {
public:
    explicit TrackedError(std::atomic<int>* count) : std::runtime_error("retained error"), count(count) {}
    ~TrackedError() override { ++*count; }
private:
    std::atomic<int>* count;
};

static void testExceptionPointer() {
    std::atomic<int> count{0};
    std::atomic<int> caught{0};
    std::exception_ptr retained;
    const TrackedError* original = nullptr;
    try {
        throw TrackedError(&count);
    } catch (const TrackedError& error) {
        original = &error;
        retained = std::current_exception();
    }
    if (!retained || count != 0) throw std::runtime_error("exception was not retained");
    std::array<std::thread, 4> threads;
    for (auto& thread : threads) {
        thread = std::thread([retained, original, &caught] {
            try {
                std::rethrow_exception(retained);
            } catch (const TrackedError& error) {
                if (&error == original && std::strcmp(error.what(), "retained error") == 0) ++caught;
            }
        });
    }
    for (auto& thread : threads) thread.join();
    if (caught != 4 || count != 0) throw std::runtime_error("cross-thread rethrow failed");
    auto copy = retained;
    retained = nullptr;
    if (count != 0) throw std::runtime_error("exception copy lost ownership");
    copy = nullptr;
    if (count != 1) throw std::runtime_error("exception destroyed an incorrect number of times");
    if (std::current_exception()) throw std::runtime_error("stale current exception");
}

int main() {
    testExceptionPointer();
    try {
        Rethrow();
        return 1;
    } catch (const std::runtime_error& error) {
        if (std::strcmp(error.what(), "native own unwind") || destroyed != 2) return 2;
    }
    try { NotImplemented_nid_no_patch("cross DLL"); }
    catch (const std::runtime_error& error) {
        if (std::strcmp(error.what(), "cross DLL not implemented")) return 3;
        try {
            Guard guard;
            throw;
        } catch (const std::logic_error&) {
            return 4;
        } catch (const std::exception& rethrown) {
            if (&rethrown != &error || destroyed != 3) return 5;
        }
        try {
            Guard guard;
            throw 42;
        } catch (const std::exception&) {
            return 6;
        } catch (...) {
            if (destroyed != 4) return 7;
            try {
                throw;
            } catch (int value) {
                if (value != 42) return 8;
            }
        }
        std::puts("Own Windows exceptions: typed catch, base catch, catch-all, rethrow, destructors, cross DLL passed");
        return 0;
    }
    return 9;
}
