//
// Created by Codex on 02/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "GizmoDrawer.hpp"

namespace LittleCore {

    struct SceneDrawerContext {
        GizmoDrawer& gizmoDrawer;
        GizmoDrawerContext& gizmoContext;
        entt::registry& cameraRegistry;
        entt::entity cameraEntity;
        entt::registry& objectRegistry;
        entt::entity objectEntity;
        ImGuizmo::OPERATION operation;
        float gameViewAspect;
    };
}
