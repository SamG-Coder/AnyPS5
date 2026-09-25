#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libSceSystemService/SystemService.hpp"
#include <SDL_misc.h>
#include <SDL_error.h>
#include <cstdlib>
#include <string>
#include <stdexcept>

extern "C" int APS5_VABI sceSystemServiceLaunchWebBrowser(const char*, const void*);
static std::string opened;
static int calls = 0;
static bool fail = false;
extern "C" int SDLCALL SDL_OpenURL(const char* url) {
    ++calls;
    opened = url;
    return fail ? -1 : 0;
}
extern "C" const char* SDLCALL SDL_GetError() { return "test browser failure"; }
static void Require(bool condition) { if (!condition) std::abort(); }
int main() {
    for (const char* url : {"http://example.com", "HTTPS://example.com/path?a=1&b=two%20words#part"}) {
        Require(sceSystemServiceLaunchWebBrowser(url, nullptr) == SYSTEM_SERVICE_OK);
        Require(opened == url);
    }
    Require(calls == 2);
    for (const char* url : {static_cast<const char*>(nullptr), "", "http://", "https:///path", "https://example.com/\n", "https://example.com/a b"})
        Require(sceSystemServiceLaunchWebBrowser(url, nullptr) == SYSTEM_SERVICE_ERROR_PARAMETER);
    for (const char* url : {"file:///tmp/test", "javascript:alert(1)", "C:\\test.exe", "example.com"}) {
        bool rejected = false;
        try { sceSystemServiceLaunchWebBrowser(url, nullptr); }
        catch (const std::runtime_error&) { rejected = true; }
        Require(rejected);
    }
    bool rejected = false;
    int options = 0;
    try { sceSystemServiceLaunchWebBrowser("https://example.com", &options); }
    catch (const std::runtime_error&) { rejected = true; }
    Require(rejected && calls == 2);
    fail = true;
    rejected = false;
    try { sceSystemServiceLaunchWebBrowser("https://example.com", nullptr); }
    catch (const std::runtime_error& error) { rejected = std::string(error.what()).find("test browser failure") != std::string::npos; }
    Require(rejected && calls == 3);
}
