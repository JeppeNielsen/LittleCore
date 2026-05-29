//
// Created by Jeppe Nielsen on 08/10/2025.
//

#include <imgui.h>
#include "ImguiInputHandler.hpp"

using namespace LittleCore;

namespace {
    InputKey FromImGuiKey(ImGuiKey key) {
        if (key >= ImGuiKey_A && key <= ImGuiKey_Z) {
            return static_cast<InputKey>(static_cast<int>(InputKey::A) + (key - ImGuiKey_A));
        }

        if (key >= ImGuiKey_1 && key <= ImGuiKey_9) {
            return static_cast<InputKey>(static_cast<int>(InputKey::N1) + (key - ImGuiKey_1));
        }

        if (key >= ImGuiKey_F1 && key <= ImGuiKey_F12) {
            return static_cast<InputKey>(static_cast<int>(InputKey::F1) + (key - ImGuiKey_F1));
        }

        if (key >= ImGuiKey_F13 && key <= ImGuiKey_F24) {
            return static_cast<InputKey>(static_cast<int>(InputKey::F13) + (key - ImGuiKey_F13));
        }

        if (key >= ImGuiKey_Keypad1 && key <= ImGuiKey_Keypad9) {
            return static_cast<InputKey>(static_cast<int>(InputKey::KP_1) + (key - ImGuiKey_Keypad1));
        }

        switch (key) {
            case ImGuiKey_0: return InputKey::N0;
            case ImGuiKey_Enter: return InputKey::RETURN;
            case ImGuiKey_Escape: return InputKey::ESCAPE;
            case ImGuiKey_Backspace: return InputKey::BACKSPACE;
            case ImGuiKey_Tab: return InputKey::TAB;
            case ImGuiKey_Space: return InputKey::SPACE;
            case ImGuiKey_Minus: return InputKey::MINUS;
            case ImGuiKey_Equal: return InputKey::EQUALS;
            case ImGuiKey_LeftBracket: return InputKey::LEFTBRACKET;
            case ImGuiKey_RightBracket: return InputKey::RIGHTBRACKET;
            case ImGuiKey_Backslash: return InputKey::BACKSLASH;
            case ImGuiKey_Semicolon: return InputKey::SEMICOLON;
            case ImGuiKey_Apostrophe: return InputKey::APOSTROPHE;
            case ImGuiKey_GraveAccent: return InputKey::GRAVE;
            case ImGuiKey_Comma: return InputKey::COMMA;
            case ImGuiKey_Period: return InputKey::PERIOD;
            case ImGuiKey_Slash: return InputKey::SLASH;
            case ImGuiKey_CapsLock: return InputKey::CAPSLOCK;
            case ImGuiKey_PrintScreen: return InputKey::PRINTSCREEN;
            case ImGuiKey_ScrollLock: return InputKey::SCROLLLOCK;
            case ImGuiKey_Pause: return InputKey::PAUSE;
            case ImGuiKey_Insert: return InputKey::INSERT;
            case ImGuiKey_Home: return InputKey::HOME;
            case ImGuiKey_PageUp: return InputKey::PAGEUP;
            case ImGuiKey_Delete: return InputKey::DELETE;
            case ImGuiKey_End: return InputKey::END;
            case ImGuiKey_PageDown: return InputKey::PAGEDOWN;
            case ImGuiKey_RightArrow: return InputKey::RIGHT;
            case ImGuiKey_LeftArrow: return InputKey::LEFT;
            case ImGuiKey_DownArrow: return InputKey::DOWN;
            case ImGuiKey_UpArrow: return InputKey::UP;
            case ImGuiKey_NumLock: return InputKey::NUMLOCKCLEAR;
            case ImGuiKey_KeypadDivide: return InputKey::KP_DIVIDE;
            case ImGuiKey_KeypadMultiply: return InputKey::KP_MULTIPLY;
            case ImGuiKey_KeypadSubtract: return InputKey::KP_MINUS;
            case ImGuiKey_KeypadAdd: return InputKey::KP_PLUS;
            case ImGuiKey_KeypadEnter: return InputKey::KP_ENTER;
            case ImGuiKey_Keypad0: return InputKey::KP_0;
            case ImGuiKey_KeypadDecimal: return InputKey::KP_PERIOD;
            case ImGuiKey_KeypadEqual: return InputKey::KP_EQUALS;
            case ImGuiKey_LeftCtrl: return InputKey::LCTRL;
            case ImGuiKey_LeftShift: return InputKey::LSHIFT;
            case ImGuiKey_LeftAlt: return InputKey::LALT;
            case ImGuiKey_LeftSuper: return InputKey::LGUI;
            case ImGuiKey_RightCtrl: return InputKey::RCTRL;
            case ImGuiKey_RightShift: return InputKey::RSHIFT;
            case ImGuiKey_RightAlt: return InputKey::RALT;
            case ImGuiKey_RightSuper: return InputKey::RGUI;
            case ImGuiKey_Menu: return InputKey::MENU;
            case ImGuiKey_AppBack: return InputKey::AC_BACK;
            case ImGuiKey_AppForward: return InputKey::AC_FORWARD;
            case ImGuiKey_Oem102: return InputKey::NONUSBACKSLASH;
            default: return InputKey::UNKNOWN;
        }
    }
}

ImguiInputHandler::ImguiInputHandler() {

}

ImguiInputHandler::~ImguiInputHandler() {

}

void ImguiInputHandler::HandleInput(void* event, Input& input) {
    input.screenSize = {(int)width, (int)height};

    Input prev = input;

    input.Clear();
    for (int i = 0; i < 10; ++i) {
        input.touchPosition[i].position = {mouseX, mouseY};
    }

    for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; ++i) {
        const auto imguiKey = static_cast<ImGuiKey>(i);
        const auto inputKey = FromImGuiKey(imguiKey);
        if (inputKey == InputKey::UNKNOWN) {
            continue;
        }

        if (ImGui::IsKeyPressed(imguiKey, false)) {
            input.keysDown.push_back(inputKey);
        }

        if (ImGui::IsKeyReleased(imguiKey)) {
            input.keysUp.push_back(inputKey);
        }
    }

    for (int i = 0; i < 3; ++i) {
        InputTouch inputTouch {i};

        if (ImGui::IsMouseClicked(i) && isMouseInWindow) {
            input.touchesDown.push_back(inputTouch);
        }

        if (ImGui::IsMouseReleased(i)) {
            input.touchesUp.push_back(inputTouch);
        }
    }
}
