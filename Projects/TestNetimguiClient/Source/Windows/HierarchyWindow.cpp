//
// Created by Jeppe Nielsen on 29/12/2024.
//

#include "HierarchyWindow.hpp"
#include <imgui.h>
#include "Hierarchy.hpp"
#include <string>
#include <LocalTransform.hpp>
#include <WorldTransform.hpp>
#include "../EditorSimulations/EditorSimulation.hpp"

using namespace LittleCore;

std::string GetEntityName(entt::entity entity) {
    return "Entity " + std::to_string((int)entity);
}

bool IsEntityInHierarchy(entt::registry& registry, entt::entity entity, entt::entity parent) {

    auto& hierarchy = registry.get<Hierarchy>(entity);
    auto find = std::find(hierarchy.children.begin(), hierarchy.children.end(), parent);

    if (find != hierarchy.children.end()) {
        return true;
    }

    for(auto child : hierarchy.children) {
        bool isInChild = IsEntityInHierarchy(registry, child, parent);
        if (isInChild) {
            return true;
        }
    }
    return false;
}

bool IsParentAllowed(entt::registry& registry, entt::entity entity, entt::entity parent) {
    if (entity == parent) {
        return false;
    }

    if (IsEntityInHierarchy(registry, entity, parent)) {
        return false;
    }

    return true;
}

void HierarchyWindow::DrawEntity(EditorSimulation& simulation, entt::entity entity, entt::entity parent) {
    auto& registry = simulation.simulation.registry;
    std::string name = GetEntityName(entity);

    Hierarchy& hierarchy = registry.get<Hierarchy>(entity);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;

    if (hierarchy.children.empty()) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (simulation.selection.IsSelected(entity)) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    nodeFlags |= ImGuiTreeNodeFlags_FramePadding;

    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
    nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool isUnfolded = ImGui::TreeNodeEx(name.c_str(), nodeFlags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        simulation.selection.Clear();
        simulation.selection.Select(entity);
    }

    if (ImGui::BeginPopupContextItem(("RightClickMenu" + std::to_string((int)entity)).c_str())) {
        if (ImGui::MenuItem("New")) {
            entitiesToCreate.push_back(entity);
        }

        if (ImGui::MenuItem("Delete")) {
            entitiesToDelete.push_back(entity);
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_AcceptBeforeDelivery)) {
        ImGui::SetDragDropPayload("DND_TREE_NODE", &entity, sizeof(entt::entity)); // Set payload
        ImGui::Text("Dragging: %s", name.c_str());
        ImGui::EndDragDropSource();
    }



    // Drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
            entt::entity draggedEntity = (*(entt::entity*)payload->Data);
            if (IsParentAllowed(registry, draggedEntity, entity)) {
                reparentedEntities.push_back({draggedEntity, entity});
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (!isUnfolded) {
        return;
    }

    for(auto child : hierarchy.children) {
        DrawEntity(simulation, child, entity);
    }

    ImGui::TreePop();
}



void HierarchyWindow::Draw(EditorSimulation& simulation) {
    ImGui::Begin("Hierarchy");

    auto& registry = simulation.simulation.registry;

    const auto& view = registry.view<Hierarchy>();

    bool isRootExpanded = ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding);

    if (ImGui::BeginPopupContextItem(("RightClickMenuRoot"))) {
        if (ImGui::MenuItem("New")) {
            entitiesToCreate.push_back(entt::null);
        }
        ImGui::EndPopup();
    }


    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE")) {
            entt::entity draggedEntity = (*(entt::entity*) payload->Data);
            reparentedEntities.push_back({draggedEntity, entt::null});
            registry.patch<Hierarchy>(draggedEntity);
        }
        ImGui::EndDragDropTarget();
    }


    if (isRootExpanded) {

        for (auto [entity, hierarchy]: view.each()) {
            if (hierarchy.parent == entt::null) {
                DrawEntity(simulation, entity, entt::null);
            }
        }

        ImGui::TreePop();
    }

    ImGui::End();

    for(auto reparentedEntity : reparentedEntities) {
        registry.get<Hierarchy>(reparentedEntity.entity).parent = reparentedEntity.newParent;
        registry.patch<Hierarchy>(reparentedEntity.entity);
    }
    reparentedEntities.clear();

    for(auto e : entitiesToCreate) {
        entt::entity newEntity = registry.create();
        registry.emplace<LocalTransform>(newEntity);
        registry.emplace<WorldTransform>(newEntity);
        registry.emplace<Hierarchy>(newEntity).parent = e;
        registry.emplace<LocalBoundingBox>(newEntity);
        registry.emplace<WorldBoundingBox>(newEntity);
        simulation.selection.Clear();
        simulation.selection.Select(e);
    }
    entitiesToCreate.clear();

    for(auto e : entitiesToDelete) {
        registry.destroy(e);
    }
    entitiesToDelete.clear();

}