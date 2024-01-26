//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "BgfxRenderer.hpp"
#include <entt/entt.hpp>
#include "HierarchySystem.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include "MeshBoundingBoxSystem.hpp"
#include "WorldTransformSystem.hpp"
#include "RenderSystem.hpp"

namespace LittleCore {
    class Simulation {
    public:
        Simulation(entt::registry &registry);

        void Update();

        void Render(BGFXRenderer& bgfxRenderer);
    private:
        entt::registry& registry;
        HierarchySystem hierarchySystem;
        MeshBoundingBoxSystem meshBoundingBoxSystem;
        WorldBoundingBoxSystem worldBoundingBoxSystem;
        WorldTransformSystem worldTransformSystem;
        RenderOctreeSystem renderOctreeSystem;
        RenderSystem renderSystem;

    };
}
