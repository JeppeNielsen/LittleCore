//
// Created by Jeppe Nielsen on 29/12/2024.
//

#include "HierarchyWindow.hpp"
#include <algorithm>
#include <imgui.h>
#include "Hierarchy.hpp"
#include <string>
#include <LocalTransform.hpp>
#include <WorldTransform.hpp>
#include "../EditorSimulations/EditorSimulation.hpp"
#include "RegistryHelper.hpp"
#include "IgnoreSerialization.hpp"

using namespace LittleCore;

namespace {
    enum class HierarchyDropMode {
        ReparentAsChild,
        InsertBeforeSibling,
        InsertAfterSibling,
    };

    HierarchyDropMode GetHierarchyDropMode(const ImVec2& itemRectMin, const ImVec2& itemRectMax) {
        const float itemHeight = itemRectMax.y - itemRectMin.y;
        const float reorderZoneHeight = std::min(6.0f, itemHeight * 0.25f);
        const float mouseY = ImGui::GetMousePos().y;

        if (mouseY <= itemRectMin.y + reorderZoneHeight) {
            return HierarchyDropMode::InsertBeforeSibling;
        }

        if (mouseY >= itemRectMax.y - reorderZoneHeight) {
            return HierarchyDropMode::InsertAfterSibling;
        }

        return HierarchyDropMode::ReparentAsChild;
    }

    void DrawHierarchyDropIndicator(const ImVec2& itemRectMin, const ImVec2& itemRectMax, HierarchyDropMode dropMode) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        const ImU32 color = ImGui::GetColorU32(ImGuiCol_DragDropTarget);

        if (dropMode == HierarchyDropMode::ReparentAsChild) {
            drawList->AddRect(itemRectMin, itemRectMax, color, 0.0f, 0, 2.0f);
            return;
        }

        const float y = dropMode == HierarchyDropMode::InsertBeforeSibling ? itemRectMin.y : itemRectMax.y;
        drawList->AddLine({itemRectMin.x, y}, {itemRectMax.x, y}, color, 2.0f);
    }

    void RemoveChild(Hierarchy::Children& children, entt::entity entity) {
        const auto found = std::find(children.begin(), children.end(), entity);
        if (found != children.end()) {
            children.erase(found);
        }
    }
}

std::string GetEntityName(entt::entity entity) {
    return "Entity " + std::to_string((uint32_t)entity);
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

bool AreAllChildrenIgnored(entt::registry& registry, const Hierarchy& hierarchy) {
    for(auto child : hierarchy.children) {
        if (!registry.any_of<IgnoreSerialization>(child)) {
            return false;
        }
    }
    return true;
}

void HierarchyWindow::QueueEntityMove(entt::registry& registry, entt::entity entity, entt::entity newParent,
                                      entt::entity anchorSibling, bool insertAfterAnchor) {
    if (!registry.valid(entity) || !registry.all_of<Hierarchy>(entity)) {
        return;
    }

    if (anchorSibling == entity) {
        return;
    }

    if (newParent != entt::null && !registry.all_of<Hierarchy>(newParent)) {
        return;
    }

    if (!IsParentAllowed(registry, entity, newParent)) {
        return;
    }

    entityMoves.push_back({entity, newParent, anchorSibling, insertAfterAnchor});
}

bool HierarchyWindow::ApplyEntityMove(entt::registry& registry, const EntityMove& move) {
    if (!registry.valid(move.entity) || !registry.all_of<Hierarchy>(move.entity)) {
        return false;
    }

    if (move.anchorSibling == move.entity) {
        return false;
    }

    if (move.newParent != entt::null && !registry.all_of<Hierarchy>(move.newParent)) {
        return false;
    }

    if (!IsParentAllowed(registry, move.entity, move.newParent)) {
        return false;
    }

    auto& movedHierarchy = registry.get<Hierarchy>(move.entity);
    const entt::entity oldParent = movedHierarchy.parent;

    if (oldParent == move.newParent && move.anchorSibling == entt::null) {
        return false;
    }

    if (oldParent != entt::null && registry.all_of<Hierarchy>(oldParent)) {
        auto& oldParentChildren = registry.get<Hierarchy>(oldParent).children;
        RemoveChild(oldParentChildren, move.entity);
        registry.patch<Hierarchy>(oldParent);
    }

    movedHierarchy.parent = move.newParent;
    movedHierarchy.previousParent = move.newParent;
    registry.patch<Hierarchy>(move.entity);

    if (move.newParent == entt::null) {
        return oldParent != move.newParent;
    }

    auto& newParentChildren = registry.get<Hierarchy>(move.newParent).children;
    RemoveChild(newParentChildren, move.entity);

    auto insertIt = newParentChildren.end();
    if (move.anchorSibling != entt::null) {
        const auto anchor = std::find(newParentChildren.begin(), newParentChildren.end(), move.anchorSibling);
        if (anchor != newParentChildren.end()) {
            insertIt = move.insertAfterAnchor ? anchor + 1 : anchor;
        }
    }

    newParentChildren.insert(insertIt, move.entity);
    registry.patch<Hierarchy>(move.newParent);
    return true;
}

void HierarchyWindow::DrawEntity(EditorSimulation& simulation, entt::entity entity, entt::entity parent) {
    auto& registry = simulation.simulation.registry;

    if (registry.any_of<IgnoreSerialization>(entity)) {
        return;
    }

    std::string name = GetEntityName(entity);

    Hierarchy& hierarchy = registry.get<Hierarchy>(entity);

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;

    if (hierarchy.children.empty() || AreAllChildrenIgnored(registry, hierarchy)) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (simulation.selection.IsSelected(entity)) {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    nodeFlags |= ImGuiTreeNodeFlags_FramePadding;

    nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
    nodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth;

    bool isUnfolded = ImGui::TreeNodeEx(name.c_str(), nodeFlags);
    const ImVec2 itemRectMin = ImGui::GetItemRectMin();
    const ImVec2 itemRectMax = ImGui::GetItemRectMax();

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

        if (ImGui::MenuItem("Duplicate")) {
            entitiesToDuplicate.push_back(entity);
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
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            entt::entity draggedEntity = (*(entt::entity*)payload->Data);
            HierarchyDropMode dropMode = parent == entt::null ? HierarchyDropMode::ReparentAsChild : GetHierarchyDropMode(itemRectMin, itemRectMax);

            if (payload->Preview) {
                DrawHierarchyDropIndicator(itemRectMin, itemRectMax, dropMode);
            }

            if (payload->Delivery) {
                switch (dropMode) {
                    case HierarchyDropMode::ReparentAsChild:
                        QueueEntityMove(registry, draggedEntity, entity);
                        break;
                    case HierarchyDropMode::InsertBeforeSibling:
                        QueueEntityMove(registry, draggedEntity, parent, entity, false);
                        break;
                    case HierarchyDropMode::InsertAfterSibling:
                        QueueEntityMove(registry, draggedEntity, parent, entity, true);
                        break;
                }
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
    const ImVec2 rootItemRectMin = ImGui::GetItemRectMin();
    const ImVec2 rootItemRectMax = ImGui::GetItemRectMax();

    if (ImGui::BeginPopupContextItem(("RightClickMenuRoot"))) {
        if (ImGui::MenuItem("New")) {
            entitiesToCreate.push_back(entt::null);
        }

        ImGui::EndPopup();
    }


    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TREE_NODE", ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            entt::entity draggedEntity = (*(entt::entity*) payload->Data);
            if (payload->Preview) {
                DrawHierarchyDropIndicator(rootItemRectMin, rootItemRectMax, HierarchyDropMode::ReparentAsChild);
            }

            if (payload->Delivery) {
                QueueEntityMove(registry, draggedEntity, entt::null);
            }
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

    bool hierarchyChanged = false;

    for (const auto& entityMove : entityMoves) {
        hierarchyChanged |= ApplyEntityMove(registry, entityMove);
    }
    entityMoves.clear();

    for(auto e : entitiesToCreate) {
        entt::entity newEntity = registry.create();
        registry.emplace<LocalTransform>(newEntity);
        registry.emplace<WorldTransform>(newEntity);
        registry.emplace<Hierarchy>(newEntity).parent = e;
        registry.emplace<LocalBoundingBox>(newEntity);
        registry.emplace<WorldBoundingBox>(newEntity);
        simulation.selection.Clear();
        simulation.selection.Select(e);
        hierarchyChanged = true;
    }
    entitiesToCreate.clear();

    for(auto e : entitiesToDelete) {
        registry.destroy(e);
        hierarchyChanged = true;
    }
    entitiesToDelete.clear();

    for(auto e : entitiesToDuplicate) {
        RegistryHelper::Duplicate(registry, e, registry);
        hierarchyChanged = true;
    }
    entitiesToDuplicate.clear();

    simulation.context.hierarchyChangedLastFrame |= hierarchyChanged;

}
