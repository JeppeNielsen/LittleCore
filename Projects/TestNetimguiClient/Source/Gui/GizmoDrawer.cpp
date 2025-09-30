//
// Created by Jeppe Nielsen on 29/09/2025.
//

#include "GizmoDrawer.hpp"
#include "Camera.hpp"
#include "WorldTransform.hpp"
#include "LocalTransform.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "Hierarchy.hpp"

using namespace LittleCore;

void GizmoDrawer::Begin() {
    min = ImGui::GetItemRectMin();
    max = ImGui::GetItemRectMax();
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(min.x, min.y, max.x - min.x, max.y - min.y);
}

void GizmoDrawer::DrawGizmo(GizmoDrawerContext& context, entt::registry& registry, entt::entity cameraEntity, entt::entity objectEntity, ImGuizmo::OPERATION operation) {
    ImVec2 size = {max.x - min.x, max.y - min.y};

    ImGuizmo::PushID((int)objectEntity);

    auto& camera = registry.get<Camera>(cameraEntity);
    auto& cameraWorldTransform = registry.get<WorldTransform>(cameraEntity);
    auto& quadWorldTransform = registry.get<WorldTransform>(objectEntity);

    auto viewPtr = glm::value_ptr(cameraWorldTransform.worldInverse);
    auto projectionPtr = glm::value_ptr(camera.GetProjection(size.x/size.y));
    auto model = glm::value_ptr(quadWorldTransform.world);
    ImGuizmo::Manipulate(viewPtr, projectionPtr, operation, ImGuizmo::MODE::LOCAL, model);

    bool isUsing = ImGuizmo::IsUsing();

    context.wasActive |= isUsing;
    context.wasHovered |= ImGuizmo::IsOver();

    if (isUsing) {
        auto& quadLocalTransform = registry.get<LocalTransform>(objectEntity);
        mat4x4 worldToDecompose;
        Hierarchy* hierarchy = registry.try_get<Hierarchy>(objectEntity);
        if (!hierarchy || !registry.valid(hierarchy->parent)) {
            worldToDecompose = quadWorldTransform.world;
        } else {
            WorldTransform& parentWorld = registry.get<WorldTransform>(hierarchy->parent);
            worldToDecompose = parentWorld.worldInverse * quadWorldTransform.world;
        }

        glm::vec3 skew;
        glm::vec4 perspective;
        if (glm::decompose(worldToDecompose, quadLocalTransform.scale, quadLocalTransform.rotation,
                            quadLocalTransform.position, skew, perspective)) {
            registry.patch<LocalTransform>(objectEntity);
        }
    }

    ImGuizmo::PopID();
}
