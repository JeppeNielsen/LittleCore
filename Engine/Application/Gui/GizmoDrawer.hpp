//
// Created by Jeppe Nielsen on 29/09/2025.
//


#pragma once
#include <entt/entt.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Math.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
#include "Sizable.hpp"

namespace LittleCore {

    struct GizmoDrawerContext {
        bool wasActive = false;
        bool wasHovered = false;
    };

    class GizmoDrawer {
    public:

        void Begin();
        void DrawGizmo(GizmoDrawerContext& context,
                       entt::registry& cameraRegistry, entt::entity cameraEntity,
                       entt::registry& objectRegistry, entt::entity objectEntity,
                       ImGuizmo::OPERATION operation);

        void DrawSizableBounds(GizmoDrawerContext& context,
                               entt::registry& cameraRegistry, entt::entity cameraEntity,
                               entt::registry& objectRegistry, entt::entity objectEntity,
                               Sizable& sizable);

        ImGuizmo::OPERATION DrawOperationUI(ImGuizmo::OPERATION currentOperation);

        void DrawCameraFrustum(entt::registry& cameraRegistry, entt::entity cameraEntity,
                               entt::registry& objectRegistry, entt::entity objectEntity, float aspect,
                               ImU32 color = IM_COL32(255,255,255,255),
                               float thickness = 1.0f,
                               bool depthZeroToOne = true);
    private:
        ImVec2 min;
        ImVec2 max;
        entt::entity activeSizableEntity = entt::null;
        glm::vec2 activeSizableSize = {0.0f, 0.0f};
        glm::vec3 activeSizableScale = {1.0f, 1.0f, 1.0f};
        glm::quat activeSizableRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    };
}
