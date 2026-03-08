//
// Created by Jeppe Nielsen on 09/02/2024.
//

#include "InputKeyMapper.hpp"
#include <sokol_app.h>

using namespace LittleCore;

InputKey InputKeyMapper::FromId(int id) {
    return (InputKey)id;
}

int InputKeyMapper::ToId(InputKey id) {
    return (int)id;
}

InputKey InputKeyMapper::FromSappKeyCode(int keyCode) {
    if (keyCode >= SAPP_KEYCODE_A && keyCode <= SAPP_KEYCODE_Z) {
        return static_cast<InputKey>(static_cast<int>(InputKey::A) + (keyCode - SAPP_KEYCODE_A));
    }

    if (keyCode >= SAPP_KEYCODE_1 && keyCode <= SAPP_KEYCODE_9) {
        return static_cast<InputKey>(static_cast<int>(InputKey::N1) + (keyCode - SAPP_KEYCODE_1));
    }

    switch (keyCode) {
        case SAPP_KEYCODE_0: return InputKey::N0;
        case SAPP_KEYCODE_ENTER: return InputKey::RETURN;
        case SAPP_KEYCODE_ESCAPE: return InputKey::ESCAPE;
        case SAPP_KEYCODE_BACKSPACE: return InputKey::BACKSPACE;
        case SAPP_KEYCODE_TAB: return InputKey::TAB;
        case SAPP_KEYCODE_SPACE: return InputKey::SPACE;
        case SAPP_KEYCODE_MINUS: return InputKey::MINUS;
        case SAPP_KEYCODE_EQUAL: return InputKey::EQUALS;
        case SAPP_KEYCODE_LEFT_BRACKET: return InputKey::LEFTBRACKET;
        case SAPP_KEYCODE_RIGHT_BRACKET: return InputKey::RIGHTBRACKET;
        case SAPP_KEYCODE_BACKSLASH: return InputKey::BACKSLASH;
        case SAPP_KEYCODE_SEMICOLON: return InputKey::SEMICOLON;
        case SAPP_KEYCODE_APOSTROPHE: return InputKey::APOSTROPHE;
        case SAPP_KEYCODE_GRAVE_ACCENT: return InputKey::GRAVE;
        case SAPP_KEYCODE_COMMA: return InputKey::COMMA;
        case SAPP_KEYCODE_PERIOD: return InputKey::PERIOD;
        case SAPP_KEYCODE_SLASH: return InputKey::SLASH;

        case SAPP_KEYCODE_CAPS_LOCK: return InputKey::CAPSLOCK;
        case SAPP_KEYCODE_F1: return InputKey::F1;
        case SAPP_KEYCODE_F2: return InputKey::F2;
        case SAPP_KEYCODE_F3: return InputKey::F3;
        case SAPP_KEYCODE_F4: return InputKey::F4;
        case SAPP_KEYCODE_F5: return InputKey::F5;
        case SAPP_KEYCODE_F6: return InputKey::F6;
        case SAPP_KEYCODE_F7: return InputKey::F7;
        case SAPP_KEYCODE_F8: return InputKey::F8;
        case SAPP_KEYCODE_F9: return InputKey::F9;
        case SAPP_KEYCODE_F10: return InputKey::F10;
        case SAPP_KEYCODE_F11: return InputKey::F11;
        case SAPP_KEYCODE_F12: return InputKey::F12;
        case SAPP_KEYCODE_F13: return InputKey::F13;
        case SAPP_KEYCODE_F14: return InputKey::F14;
        case SAPP_KEYCODE_F15: return InputKey::F15;
        case SAPP_KEYCODE_F16: return InputKey::F16;
        case SAPP_KEYCODE_F17: return InputKey::F17;
        case SAPP_KEYCODE_F18: return InputKey::F18;
        case SAPP_KEYCODE_F19: return InputKey::F19;
        case SAPP_KEYCODE_F20: return InputKey::F20;
        case SAPP_KEYCODE_F21: return InputKey::F21;
        case SAPP_KEYCODE_F22: return InputKey::F22;
        case SAPP_KEYCODE_F23: return InputKey::F23;
        case SAPP_KEYCODE_F24: return InputKey::F24;

        case SAPP_KEYCODE_PRINT_SCREEN: return InputKey::PRINTSCREEN;
        case SAPP_KEYCODE_SCROLL_LOCK: return InputKey::SCROLLLOCK;
        case SAPP_KEYCODE_PAUSE: return InputKey::PAUSE;
        case SAPP_KEYCODE_INSERT: return InputKey::INSERT;
        case SAPP_KEYCODE_HOME: return InputKey::HOME;
        case SAPP_KEYCODE_PAGE_UP: return InputKey::PAGEUP;
        case SAPP_KEYCODE_DELETE: return InputKey::DELETE;
        case SAPP_KEYCODE_END: return InputKey::END;
        case SAPP_KEYCODE_PAGE_DOWN: return InputKey::PAGEDOWN;
        case SAPP_KEYCODE_RIGHT: return InputKey::RIGHT;
        case SAPP_KEYCODE_LEFT: return InputKey::LEFT;
        case SAPP_KEYCODE_DOWN: return InputKey::DOWN;
        case SAPP_KEYCODE_UP: return InputKey::UP;

        case SAPP_KEYCODE_NUM_LOCK: return InputKey::NUMLOCKCLEAR;
        case SAPP_KEYCODE_KP_DIVIDE: return InputKey::KP_DIVIDE;
        case SAPP_KEYCODE_KP_MULTIPLY: return InputKey::KP_MULTIPLY;
        case SAPP_KEYCODE_KP_SUBTRACT: return InputKey::KP_MINUS;
        case SAPP_KEYCODE_KP_ADD: return InputKey::KP_PLUS;
        case SAPP_KEYCODE_KP_ENTER: return InputKey::KP_ENTER;
        case SAPP_KEYCODE_KP_1: return InputKey::KP_1;
        case SAPP_KEYCODE_KP_2: return InputKey::KP_2;
        case SAPP_KEYCODE_KP_3: return InputKey::KP_3;
        case SAPP_KEYCODE_KP_4: return InputKey::KP_4;
        case SAPP_KEYCODE_KP_5: return InputKey::KP_5;
        case SAPP_KEYCODE_KP_6: return InputKey::KP_6;
        case SAPP_KEYCODE_KP_7: return InputKey::KP_7;
        case SAPP_KEYCODE_KP_8: return InputKey::KP_8;
        case SAPP_KEYCODE_KP_9: return InputKey::KP_9;
        case SAPP_KEYCODE_KP_0: return InputKey::KP_0;
        case SAPP_KEYCODE_KP_DECIMAL: return InputKey::KP_PERIOD;
        case SAPP_KEYCODE_KP_EQUAL: return InputKey::KP_EQUALS;

        case SAPP_KEYCODE_LEFT_CONTROL: return InputKey::LCTRL;
        case SAPP_KEYCODE_LEFT_SHIFT: return InputKey::LSHIFT;
        case SAPP_KEYCODE_LEFT_ALT: return InputKey::LALT;
        case SAPP_KEYCODE_LEFT_SUPER: return InputKey::LGUI;
        case SAPP_KEYCODE_RIGHT_CONTROL: return InputKey::RCTRL;
        case SAPP_KEYCODE_RIGHT_SHIFT: return InputKey::RSHIFT;
        case SAPP_KEYCODE_RIGHT_ALT: return InputKey::RALT;
        case SAPP_KEYCODE_RIGHT_SUPER: return InputKey::RGUI;
        case SAPP_KEYCODE_MENU: return InputKey::MENU;

        default:
            return InputKey::UNKNOWN;
    }
}
