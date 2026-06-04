//
// Created by Jeppe Nielsen on 09/10/2025.
//

#include <imgui.h>
#include "GuiWindowInputController.hpp"
#include "ImguiInputHandler.hpp"

using namespace LittleCore;

void GuiWindowInputController::Begin() {
    auto gameViewMin = ImGui::GetCursorScreenPos();

    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    gameViewMinX = gameViewMin.x;
    gameViewMinY = gameViewMin.y;
    width = windowSize.x;
    height = windowSize.y;

    wasWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
}
void GuiWindowInputController::RunforSimulation(LittleCore::SimulationBase& simulation) {
    if (!wasWindowFocused) {
        return;
    }

    ImguiInputHandler inputHandler;

    ImVec2 mousePosition = ImGui::GetMousePos();
    inputHandler.mouseX = mousePosition.x - gameViewMinX;
    inputHandler.mouseY = mousePosition.y - gameViewMinY;
    inputHandler.width = width;
    inputHandler.height = height;

    inputHandler.isMouseInWindow =
            mousePosition.x >= gameViewMinX &&
            mousePosition.x <= gameViewMinX + width &&
            mousePosition.y >= gameViewMinY &&
            mousePosition.y <= gameViewMinY + height;
    simulation.HandleEvent(nullptr, inputHandler);

}
