//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "LocalBoundingBoxEditor.hpp"
#include "../GuiHelper.hpp"

bool LocalBoundingBoxEditor::Draw(entt::registry& registry, entt::entity entity, LittleCore::LocalBoundingBox& component) {
    bool changed = false;
    GuiHelper::DrawHeader("Bounding Box");
    GuiHelper::Draw(changed, "Center", component.bounds.center);
    GuiHelper::Draw(changed, "Extends", component.bounds.extends);
    return changed;
}
