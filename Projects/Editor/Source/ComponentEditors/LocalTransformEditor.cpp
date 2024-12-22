//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "LocalTransformEditor.hpp"
#include "GuiHelper.hpp"

void LocalTransformEditor::Draw(entt::registry &registry, entt::entity entity, LittleCore::LocalTransform &component) {
    GuiHelper::DrawHeader("Transform");
    GuiHelper::Draw("Position", component.position);
    GuiHelper::Draw("Rotation", component.rotation);
    GuiHelper::Draw("Scale", component.scale);
}
