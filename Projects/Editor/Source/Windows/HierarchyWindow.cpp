//
// Created by Jeppe Nielsen on 29/12/2024.
//

#include "HierarchyWindow.hpp"
#include <imgui.h>
#include "Hierarchy.hpp"

using namespace LittleCore;

HierarchyWindow::HierarchyWindow(EngineContext& engineContext) : engineContext(engineContext) {

}

std::string GetEntityName(entt::entity entity) {
    return "Entity " + std::to_string((int)entity);
}

void DrawEntity(entt::registry& registry, entt::entity entity, entt::entity parent) {

    std::string name = GetEntityName(entity);

    if (!ImGui::TreeNode(name.c_str())) {
        return;
    }

    Hierarchy& hierarchy = registry.get<Hierarchy>(entity);

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_AcceptBeforeDelivery)) {
        ImGui::SetDragDropPayload("DND_TREE_NODE", &entity, sizeof(entt::entity)); // Set payload
        ImGui::Text("Dragging: %s", name.c_str());
        ImGui::EndDragDropSource();
    }

    // Drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
            entt::entity draggedEntity = (*(entt::entity*)payload->Data);
            if (draggedEntity != entity) {
                Hierarchy& draggedHierarchy = registry.get<Hierarchy>(draggedEntity);
                draggedHierarchy.parent = entity;
                registry.patch<Hierarchy>(draggedEntity);
            }
        }
        ImGui::EndDragDropTarget();
    }


    for(auto child : hierarchy.children) {
        DrawEntity(registry, child, entity);
    }

    ImGui::TreePop();
}



void HierarchyWindow::DrawGui() {
    ImGui::Begin("Hierarchy");

    if (engineContext.registryCollection->registries.empty()) {
        ImGui::Text("No registries found");
        ImGui::End();
        return;
    }

    auto& registry = *engineContext.registryCollection->registries[0];

    const auto& view = registry.view<Hierarchy>();

    if (ImGui::TreeNode("Registry")) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
                entt::entity draggedEntity = (*(entt::entity*) payload->Data);
                Hierarchy& draggedHierarchy = registry.get<Hierarchy>(draggedEntity);
                draggedHierarchy.parent = entt::null;
                registry.patch<Hierarchy>(draggedEntity);
            }
            ImGui::EndDragDropTarget();
        }

        for (auto [entity, hierarchy]: view.each()) {
            if (hierarchy.parent == entt::null) {
                DrawEntity(registry, entity, entt::null);
            }
        }

        ImGui::TreePop();
    }






    ImGui::End();
}
