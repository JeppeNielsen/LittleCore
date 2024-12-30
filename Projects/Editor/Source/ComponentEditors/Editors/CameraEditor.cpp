//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "CameraEditor.hpp"
#include "../GuiHelper.hpp"
#include <imgui.h>

bool CameraEditor::Draw(entt::registry &registry, entt::entity entity, LittleCore::Camera& component) {
    bool changed = false;
    GuiHelper::DrawHeader("Camera");
    GuiHelper::Draw(changed, "Clear background", component.clearBackground);
    GuiHelper::DrawColor(changed, "Clear background", component.clearColor);
    GuiHelper::Draw(changed,"Near", component.near);
    GuiHelper::Draw(changed,"Far", component.far);

    GuiHelper::Draw(changed, "Orthographic", component.isOrthographic);

    if (component.IsOrthographic()) {
        GuiHelper::Draw(changed, "Orthographic size", component.orthoSize);
    } else {
        GuiHelper::Draw(changed, "Field of view", component.fieldOfView);
    }

    GuiHelper::Draw(changed, "View rect", component.viewRect);
    return changed;
}
