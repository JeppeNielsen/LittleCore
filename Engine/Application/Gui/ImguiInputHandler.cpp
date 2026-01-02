//
// Created by Jeppe Nielsen on 08/10/2025.
//

#include <imgui.h>
#include "ImguiInputHandler.hpp"
#include <iostream>

using namespace LittleCore;

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

    ImGuiIO& io = ImGui::GetIO();

    for (int i = 0; i < ImGuiKey_KeysData_SIZE; ++i) {
        if (ImGui::IsKeyPressed((ImGuiKey)i, false)) {
            input.keysDown.push_back((InputKey)i);
        }

        if (ImGui::IsKeyReleased((ImGuiKey)i)) {
            input.keysUp.push_back((InputKey)i);
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
