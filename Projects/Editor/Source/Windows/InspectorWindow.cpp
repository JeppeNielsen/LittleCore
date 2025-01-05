//
// Created by Jeppe Nielsen on 01/01/2025.
//

#include "InspectorWindow.hpp"
#include <imgui.h>

InspectorWindow::InspectorWindow(RegistryManager& registryManager,
                                 ComponentDrawer& componentDrawer,
                                 ComponentFactory& componentFactory)
                                 : registryManager(registryManager), componentDrawer(componentDrawer), componentFactory(componentFactory) {

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
    componentDrawer.Draw(registry, state.selectedEntity);

    auto ids = defaultComponentEditors.GetComponentIds();
    auto customIds = componentFactory.GetComponentIds();
    ids.insert(ids.begin(), customIds.begin(), customIds.end());

    if (ImGui::BeginPopupContextWindow("ComponentCreateMenu")) {

        for(auto componentId : ids) {
            if (defaultComponentEditors.DoesEntityHaveComponent(registry, state.selectedEntity, componentId)) {
                continue;
            }
            std::string id = "Add " + componentId;

            if (ImGui::MenuItem(id.c_str())) {
                defaultComponentEditors.CreateComponent(registry, state.selectedEntity, componentId);
                componentFactory.CreateComponent(registry, state.selectedEntity, componentId);
            }
        }

        ImGui::EndPopup();
    }



    ImGui::End();
}

