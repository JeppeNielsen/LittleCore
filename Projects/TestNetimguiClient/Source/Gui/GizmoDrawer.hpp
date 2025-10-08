//
// Created by Jeppe Nielsen on 29/09/2025.
//


#pragma once
#include <entt/entt.hpp>
#include "imgui.h"
#include "ImGuizmo.h"

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

        ImGuizmo::OPERATION DrawOperationUI(ImGuizmo::OPERATION currentOperation);

        void DrawCameraFrustum(entt::registry& cameraRegistry, entt::entity cameraEntity,
                               entt::registry& objectRegistry, entt::entity objectEntity, float aspect,
                               ImU32 color = IM_COL32(255,255,255,255),
                               float thickness = 1.0f,
                               bool depthZeroToOne = true);
    private:
        ImVec2 min;
        ImVec2 max;

    };
}