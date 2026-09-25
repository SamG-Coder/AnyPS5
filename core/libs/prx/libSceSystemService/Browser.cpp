#include "prx/libc/include/general/VabiMacros.hpp"
#include "prx/libSceSystemService/SystemService.hpp"
#include <SDL_misc.h>
#include <SDL_error.h>
#include <string>
#include <string_view>
#include <stdexcept>

extern "C" int APS5_VABI sceSystemServiceLaunchWebBrowser(const char* uri, const void* options) {
    if (!uri || !*uri) return SYSTEM_SERVICE_ERROR_PARAMETER;
    if (options) throw std::runtime_error("sceSystemServiceLaunchWebBrowser: unsupported options");
    const std::string_view url(uri);
    const auto separator = url.find("://");
    std::string scheme(url.substr(0, separator));
    for (auto& character : scheme)
        if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
    if (separator == std::string_view::npos || (scheme != "http" && scheme != "https"))
        throw std::runtime_error("sceSystemServiceLaunchWebBrowser: unsupported URL scheme");
    const auto authority = url.substr(separator + 3, url.find_first_of("/?#", separator + 3) - (separator + 3));
    if (authority.empty()) return SYSTEM_SERVICE_ERROR_PARAMETER;
    for (const unsigned char character : url)
        if (character <= 32 || character == 127) return SYSTEM_SERVICE_ERROR_PARAMETER;
    if (SDL_OpenURL(uri) != 0)
        throw std::runtime_error(std::string("sceSystemServiceLaunchWebBrowser: ") + SDL_GetError());
    return SYSTEM_SERVICE_OK;
}
