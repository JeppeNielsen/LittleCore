//
// Created by Jeppe Nielsen on 09/10/2025.
//

#include <imgui.h>
#include "GuiWindowInputController.hpp"
#include "ImguiInputHandler.hpp"

using namespace LittleCore;

void GuiWindowInputController::RunforSimulation(LittleCore::SimulationBase& simulation) {
    ImVec2 gameViewMin = ImGui::GetItemRectMin();
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        ImguiInputHandler inputHandler;

        ImVec2 mousePosition = ImGui::GetMousePos();
        inputHandler.mouseX = mousePosition.x - gameViewMin.x;
        inputHandler.mouseY = mousePosition.y - gameViewMin.y;

        inputHandler.isMouseInWindow = ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows);
        simulation.HandleEvent(nullptr, inputHandler);
    }
}
