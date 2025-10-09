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
    Input prev = input;

    input.Clear();
    for (int i = 0; i < 10; ++i) {
        input.touchPosition[i].position = {mouseX, mouseY};
    }

    ImGuiIO& io = ImGui::GetIO();

    for (int i = 0; i < ImGuiKey_KeysData_SIZE; ++i) {
        if (ImGui::IsKeyPressed((ImGuiKey)i, false)) {
            input.keysDown.push_back((InputKey)i);
            std::cout << "Key down " << i << "\n";
        }

        if (ImGui::IsKeyReleased((ImGuiKey)i)) {
            input.keysUp.push_back((InputKey)i);
            std::cout << "Key up " << i << "\n";
        }

        /*
        if (io.KeysDown[i] && std::find(prev.keysDown.begin(), prev.keysDown.end(),(InputKey)i) == prev.keysDown.end()) {
            input.keysDown.push_back((InputKey)i);
            std::cout << "Key down " << i << "\n";
        } else if (!io.KeysDown[i] && std::find(prev.keysUp.begin(), prev.keysUp.end(),(InputKey)i) == prev.keysDown.end()) {
            input.keysUp.push_back((InputKey)i);
            std::cout << "Key up " << i << "\n";
        }
         */
    }

    for (int i = 0; i < 3; ++i) {
        InputTouch inputTouch {i};

        if (ImGui::IsMouseClicked(i) && isMouseInWindow) {
            input.touchesDown.push_back(inputTouch);
            std::cout << "Mouse down " << i << "\n";
        }

        if (ImGui::IsMouseReleased(i)) {
            input.touchesUp.push_back(inputTouch);
            std::cout << "Mouse up " << i << "\n";
        }

    }
}
