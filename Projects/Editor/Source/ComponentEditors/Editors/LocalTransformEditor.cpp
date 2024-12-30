//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "LocalTransformEditor.hpp"
#include "../GuiHelper.hpp"

bool LocalTransformEditor::Draw(entt::registry &registry, entt::entity entity, LittleCore::LocalTransform &component) {
    bool changed = false;
    GuiHelper::DrawHeader("Transform");
    GuiHelper::Draw(changed, "Position", component.position);
    GuiHelper::Draw(changed,"Rotation", component.rotation);
    GuiHelper::Draw(changed,"Scale", component.scale);
    return changed;
}
