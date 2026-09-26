#include "prx/libc/include/General.hpp"
#include <exception>
#include <future>
#include <stdexcept>
#include <string>

int main() {
    std::exception_ptr captured;
    try {
        NotImplemented_nid_no_patch("native-host-exception-regression");
    } catch (const std::runtime_error& error) {
        if (std::string(error.what()).find("native-host-exception-regression") == std::string::npos) return 1;
        captured = std::current_exception();
    }
    if (!captured) return 2;
    auto copy = captured;
    auto result = std::async(std::launch::async, [copy] {
        try { std::rethrow_exception(copy); }
        catch (const std::runtime_error& error) {
            return std::string(error.what()).find("native-host-exception-regression") != std::string::npos;
        }
        return false;
    });
    if (!result.get()) return 3;
    auto made = std::make_exception_ptr(std::runtime_error("constructed exception"));
    auto madeResult = std::async(std::launch::async, [made] {
        try { std::rethrow_exception(made); }
        catch (const std::runtime_error& error) {
            return std::string(error.what()) == "constructed exception";
        }
        return false;
    });
    if (!madeResult.get()) return 5;
    unsigned caught = 0;
    for (unsigned i = 0; i < 100; ++i) {
        try { throw std::invalid_argument("host allocation/free"); }
        catch (const std::exception&) { ++caught; }
    }
    return caught == 100 ? 0 : 4;
}
