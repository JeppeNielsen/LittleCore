//
// Created by Jeppe Nielsen on 29/09/2025.
//


#pragma once
#include <entt/entt.hpp>
#include "imgui.h"
#include "ImGuizmo.h"

namespace LittleCore {
    class GizmoDrawer {
    public:

        void DrawGizmo(entt::registry& registry, entt::entity cameraEntity, entt::entity objectEntity, ImGuizmo::OPERATION operation);

    };
}