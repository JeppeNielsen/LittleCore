//
// Created by Jeppe Nielsen on 06/10/2025.
//
#include <LocalTransform.hpp>
#include "GuiHelper.hpp"
#include "InspectorWindow.hpp"
#include "imgui.h"
#include "../EditorSimulations//EditorSimulation.hpp"
#include "EntityGuiDrawer.hpp"

using namespace LittleCore;

void InspectorWindow::Draw(EditorSimulation& simulation) {

    ImGui::Begin("Inspector");
    for (auto entity : simulation.selection.selection) {
        if (simulation.simulation.registry.valid(entity)) {
            simulation.context.hierarchyChangedLastFrame |= DrawEntity(simulation, entity);
        }
    }
    ImGui::End();

}

bool InspectorWindow::DrawEntity(EditorSimulation& simulation, entt::entity e) {

    ImGui::PushID((int)e);

    //auto text = std::format("Entity: {}", (uint32_t)e);

    std::string text = "Entity: " + (uint32_t)e;

    GuiHelper::DrawHeader(text.c_str());
    bool didChange = simulation.context.guiDrawer->Draw(simulation.simulation.registry, e);

    ImGui::PopID();

    didChange |= simulation.context.guiDrawer->DrawComponentMenu(simulation.simulation.registry, e);

    return didChange;
}

