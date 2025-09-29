//
// Created by Jeppe Nielsen on 29/09/2025.
//

#include "GizmoDrawer.hpp"
#include "Camera.hpp"
#include "WorldTransform.hpp"
#include "LocalTransform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

using namespace LittleCore;

void LittleCore::GizmoDrawer::DrawGizmo(entt::registry& registry, entt::entity cameraEntity, entt::entity objectEntity, ImGuizmo::OPERATION operation) {

    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    ImVec2 size = {max.x - min.x, max.y - min.y};

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(min.x, min.y, max.x - min.x, max.y - min.y);

    auto& camera = registry.get<Camera>(cameraEntity);
    auto& cameraWorldTransform = registry.get<WorldTransform>(cameraEntity);
    auto& quadWorldTransform = registry.get<WorldTransform>(objectEntity);

    auto viewPtr = glm::value_ptr(cameraWorldTransform.worldInverse);
    auto projectionPtr = glm::value_ptr(camera.GetProjection(size.x/size.y));
    auto model = glm::value_ptr(quadWorldTransform.world);
    ImGuizmo::Manipulate(viewPtr, projectionPtr, operation, ImGuizmo::MODE::LOCAL, model);

    auto& quadLocalTransform = registry.get<LocalTransform>(objectEntity);
    glm::vec3 skew;
    glm::vec4 perspective;
    if (!glm::decompose(quadWorldTransform.world, quadLocalTransform.scale, quadLocalTransform.rotation, quadLocalTransform.position, skew, perspective)){
        return;
    }

    registry.patch<LocalTransform>(objectEntity);
}
