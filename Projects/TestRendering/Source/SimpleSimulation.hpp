//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "BgfxRenderer.hpp"
#include <entt/entt.hpp>
#include <InputSystem.hpp>
#include "HierarchySystem.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include "MeshBoundingBoxSystem.hpp"
#include "WorldTransformSystem.hpp"
#include "RenderSystem.hpp"

namespace LittleCore {
    class SimpleSimulation {
    public:
        SimpleSimulation(entt::registry &registry);

        void Update();

        void Render(BGFXRenderer &bgfxRenderer);

        InputSystem& Input();
    private:
        entt::registry& registry;
        HierarchySystem hierarchySystem;
        MeshBoundingBoxSystem meshBoundingBoxSystem;
        WorldBoundingBoxSystem worldBoundingBoxSystem;
        WorldTransformSystem worldTransformSystem;
        RenderSystem renderSystem;
        InputSystem inputSystem;
    };
}
