//
// Created by Jeppe Nielsen on 06/10/2025.
//
#include <LocalTransform.hpp>
#include "GuiHelper.hpp"
#include "InspectorWindow.hpp"
#include "imgui.h"
#include "../EditorSimulations//EditorSimulation.hpp"
#include "EntityGuiDrawer.hpp"
#include "MathReflection.hpp"

using namespace LittleCore;

void InspectorWindow::Draw(EditorSimulation& simulation) {

    ImGui::Begin("Inspector");
    for (auto entity : simulation.selection.selection) {
        if (simulation.simulation.registry.valid(entity)) {
            DrawEntity(simulation, entity);
        }
    }
    ImGui::End();

}

void InspectorWindow::DrawEntity(EditorSimulation& simulation, entt::entity e) {

    EntityGuiDrawer<LocalTransform, Camera> drawer;

    ImGui::PushID((int)e);

    GuiHelper::DrawHeader("Entity:");
    simulation.context.guiDrawer.Draw(simulation.simulation.registry, e);

    ImGui::PopID();

    simulation.context.guiDrawer.DrawComponentMenu(simulation.simulation.registry, e);
}

