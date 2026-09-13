#include <stdexcept>
#include <cstdio>
#include <cstring>

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
int main() {
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
