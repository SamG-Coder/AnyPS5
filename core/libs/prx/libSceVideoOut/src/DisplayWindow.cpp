#include "prx/libSceVideoOut/include/DisplayWindow.hpp"
#include "prx/libSceAgcDriver/Execution/include/AspectFit.hpp"
#include "prx/libkernel/AppMetadata/include/AppMetadata.hpp"
#include "SDL_vulkan.h"
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include "SDL_syswm.h"
#include <windows.h>
#include <commctrl.h>
#endif

namespace {

void require(bool condition, const char* reason) {
    if (!condition) throw std::runtime_error(std::string("DisplayWindow: ") + reason);
}

#ifdef _WIN32
constexpr UINT_PTR DisplayWindowSubclassId = 0x41505335u;
#endif

}

DisplayWindow::~DisplayWindow() {
    Destroy();
}

void DisplayWindow::Ensure(std::uint32_t sourceWidth, std::uint32_t sourceHeight) {
    require(sourceWidth != 0 && sourceHeight != 0, "source extent must be non-zero");
    if (window == nullptr) create(sourceWidth, sourceHeight);
    updateAspectRatio(sourceWidth, sourceHeight);
}

void DisplayWindow::create(std::uint32_t sourceWidth, std::uint32_t sourceHeight) {
    SDL_Rect usable{};
    require(SDL_GetDisplayUsableBounds(0, &usable) == 0, SDL_GetError());
    std::uint32_t initialWidth = sourceWidth;
    std::uint32_t initialHeight = sourceHeight;
    if (usable.w > 0 && usable.h > 0 && (sourceWidth > static_cast<std::uint32_t>(usable.w) || sourceHeight > static_cast<std::uint32_t>(usable.h))) {
        const auto fitted = AgcDriver::ComputeContainSize_nid_postfix(sourceWidth, sourceHeight, static_cast<std::uint32_t>(usable.w), static_cast<std::uint32_t>(usable.h), false);
        initialWidth = fitted.width;
        initialHeight = fitted.height;
    }
    const auto title = GetAppTitle_nid_postfix();
    window = SDL_CreateWindow(title.value, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(initialWidth), static_cast<int>(initialHeight), SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    require(window != nullptr, SDL_GetError());
    SDL_SetWindowMinimumSize(window, static_cast<int>(DisplayWindowMinimumWidth), static_cast<int>(DisplayWindowMinimumHeight));
    installSubclass();
}

void DisplayWindow::updateAspectRatio(std::uint32_t sourceWidth, std::uint32_t sourceHeight) {
    aspectWidth = sourceWidth;
    aspectHeight = sourceHeight;
}

void DisplayWindow::Destroy() noexcept {
    if (window == nullptr) return;
    removeSubclass();
    SDL_DestroyWindow(window);
    window = nullptr;
}

SDL_Window* DisplayWindow::Handle() const {
    return window;
}

void DisplayWindow::DrawableSize(std::uint32_t& width, std::uint32_t& height) const {
    if (window == nullptr) {
        width = 0;
        height = 0;
        return;
    }
    int drawableWidth = 0;
    int drawableHeight = 0;
    SDL_Vulkan_GetDrawableSize(window, &drawableWidth, &drawableHeight);
    width = drawableWidth > 0 ? static_cast<std::uint32_t>(drawableWidth) : 0;
    height = drawableHeight > 0 ? static_cast<std::uint32_t>(drawableHeight) : 0;
}

void DisplayWindow::installSubclass() {
#ifdef _WIN32
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    require(SDL_GetWindowWMInfo(window, &info) == SDL_TRUE, SDL_GetError());
    require(info.subsystem == SDL_SYSWM_WINDOWS, "unsupported window subsystem");
    const auto attached = SetWindowSubclass(info.info.win.window, reinterpret_cast<SUBCLASSPROC>(&DisplayWindow::windowProc), DisplayWindowSubclassId, reinterpret_cast<DWORD_PTR>(this));
    require(attached != FALSE, "SetWindowSubclass failed");
#endif
}

void DisplayWindow::removeSubclass() noexcept {
#ifdef _WIN32
    if (window == nullptr) return;
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(window, &info) != SDL_TRUE) return;
    if (info.subsystem != SDL_SYSWM_WINDOWS) return;
    RemoveWindowSubclass(info.info.win.window, reinterpret_cast<SUBCLASSPROC>(&DisplayWindow::windowProc), DisplayWindowSubclassId);
#endif
}

void DisplayWindow::applyAspectRatio(std::uintptr_t edge, void* rect) const {
#ifdef _WIN32
    if (aspectWidth == 0 || aspectHeight == 0) return;
    auto* bounds = static_cast<RECT*>(rect);
    const auto currentWidth = static_cast<std::uint32_t>(bounds->right - bounds->left);
    const auto currentHeight = static_cast<std::uint32_t>(bounds->bottom - bounds->top);
    if (edge == WMSZ_TOP || edge == WMSZ_BOTTOM) {
        const auto width = AgcDriver::ComputeWidthForHeight_nid_postfix(aspectWidth, aspectHeight, currentHeight);
        bounds->right = bounds->left + static_cast<LONG>(width);
        return;
    }
    const auto height = AgcDriver::ComputeHeightForWidth_nid_postfix(aspectWidth, aspectHeight, currentWidth);
    if (edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT) bounds->top = bounds->bottom - static_cast<LONG>(height);
    else bounds->bottom = bounds->top + static_cast<LONG>(height);
#else
    static_cast<void>(edge);
    static_cast<void>(rect);
#endif
}

std::intptr_t __stdcall DisplayWindow::windowProc(void* hwnd, unsigned int message, std::uintptr_t wParam, std::intptr_t lParam, std::uintptr_t subclassId, std::uintptr_t referenceData) {
#ifdef _WIN32
    static_cast<void>(subclassId);
    if (message == WM_SIZING) {
        reinterpret_cast<const DisplayWindow*>(referenceData)->applyAspectRatio(wParam, reinterpret_cast<void*>(lParam));
    }
    return DefSubclassProc(static_cast<HWND>(hwnd), message, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
#else
    static_cast<void>(hwnd);
    static_cast<void>(message);
    static_cast<void>(wParam);
    static_cast<void>(lParam);
    static_cast<void>(subclassId);
    static_cast<void>(referenceData);
    return 0;
#endif
}
