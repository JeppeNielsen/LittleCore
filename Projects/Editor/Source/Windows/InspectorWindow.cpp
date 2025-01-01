//
// Created by Jeppe Nielsen on 01/01/2025.
//

#include "InspectorWindow.hpp"
#include <imgui.h>

InspectorWindow::InspectorWindow(RegistryManager& registryManager,
                                 InspectorWindow::DrawCustomFunction drawCustomFunction)
                                 : registryManager(registryManager), drawCustomFunction(drawCustomFunction) {

}

void InspectorWindow::DrawGui() {
    ImGui::Begin("Inspector");

    std::string firstId;
    if (!registryManager.TryGetFirstId(firstId)) {
        ImGui::Text("No registries found");
        ImGui::End();
        return;
    }

    auto& state = registryManager.Get(firstId);

    if (state.registry.expired()) {
        ImGui::Text("Registry with id %s not valid anymore", firstId.c_str());
        ImGui::End();
        return;
    }

    auto& registry = *state.registry.lock().get();

    if (!registry.valid(state.selectedEntity)) {
        ImGui::End();
        return;
    }

    defaultComponentEditors.Draw(registry, state.selectedEntity);
    drawCustomFunction(registry, state.selectedEntity);

    ImGui::End();
}

