#ifndef CORE_LIBS_PRX_LIBSCEPAD_INPUTMAPPING_HPP
#define CORE_LIBS_PRX_LIBSCEPAD_INPUTMAPPING_HPP

#include <array>
#include <cstdint>
#include "SDL_scancode.h"
#include "SDL_mouse.h"

namespace Pad {
enum class InputControl {
    Button,
    LeftStickLeft,
    LeftStickRight,
    LeftStickUp,
    LeftStickDown,
    RightStickLeft,
    RightStickRight,
    RightStickUp,
    RightStickDown,
    TouchLeft,
    TouchRight,
    ToggleMouse,
    ToggleFullscreen
};

struct InputBinding {
    SDL_Scancode key;
    std::uint8_t mouseButton;
    InputControl control;
    std::uint32_t button = 0;
    int wheelDirection = 0;
};

inline constexpr std::array InputMapping{
    InputBinding{SDL_SCANCODE_F11, 0, InputControl::ToggleFullscreen},
    InputBinding{SDL_SCANCODE_RETURN, 0, InputControl::Button, 0x4000},
    InputBinding{SDL_SCANCODE_ESCAPE, 0, InputControl::Button, 0x8},
    InputBinding{SDL_SCANCODE_I, 0, InputControl::Button, 0x1000},
    InputBinding{SDL_SCANCODE_C, 0, InputControl::Button, 0x2000},
    InputBinding{SDL_SCANCODE_SPACE, 0, InputControl::Button, 0x8000},
    InputBinding{SDL_SCANCODE_Q, 0, InputControl::Button, 0x400},
    InputBinding{SDL_SCANCODE_E, 0, InputControl::Button, 0x800},
    InputBinding{SDL_SCANCODE_LSHIFT, 0, InputControl::Button, 0x2},
    InputBinding{SDL_SCANCODE_RSHIFT, 0, InputControl::Button, 0x2},
    InputBinding{SDL_SCANCODE_LCTRL, 0, InputControl::Button, 0x4},
    InputBinding{SDL_SCANCODE_RCTRL, 0, InputControl::Button, 0x4},
    InputBinding{SDL_SCANCODE_UP, 0, InputControl::Button, 0x10},
    InputBinding{SDL_SCANCODE_RIGHT, 0, InputControl::Button, 0x20},
    InputBinding{SDL_SCANCODE_DOWN, 0, InputControl::Button, 0x40},
    InputBinding{SDL_SCANCODE_LEFT, 0, InputControl::Button, 0x80},
    InputBinding{SDL_SCANCODE_A, 0, InputControl::LeftStickLeft},
    InputBinding{SDL_SCANCODE_D, 0, InputControl::LeftStickRight},
    InputBinding{SDL_SCANCODE_W, 0, InputControl::LeftStickUp},
    InputBinding{SDL_SCANCODE_S, 0, InputControl::LeftStickDown},
    InputBinding{SDL_SCANCODE_F, 0, InputControl::RightStickLeft},
    InputBinding{SDL_SCANCODE_H, 0, InputControl::RightStickRight},
    InputBinding{SDL_SCANCODE_T, 0, InputControl::RightStickUp},
    InputBinding{SDL_SCANCODE_G, 0, InputControl::RightStickDown},
    InputBinding{SDL_SCANCODE_BACKSPACE, 0, InputControl::TouchLeft},
    InputBinding{SDL_SCANCODE_TAB, 0, InputControl::TouchRight},
    InputBinding{SDL_SCANCODE_UNKNOWN, SDL_BUTTON_MIDDLE, InputControl::ToggleMouse},
    InputBinding{SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LEFT, InputControl::Button, 0x200},
    InputBinding{SDL_SCANCODE_UNKNOWN, SDL_BUTTON_RIGHT, InputControl::Button, 0x800},
    InputBinding{SDL_SCANCODE_UNKNOWN, 0, InputControl::Button, 0x10, 1},
    InputBinding{SDL_SCANCODE_UNKNOWN, 0, InputControl::Button, 0x40, -1}
};

inline constexpr int MousePollIntervalMs = 33;
inline constexpr int WheelPressDurationMs = 80;
inline constexpr double MouseSensitivity = 1.0;
}

#endif
