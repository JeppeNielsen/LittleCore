//
// Created by Jeppe Nielsen on 21/01/2024.
//

#pragma once
#include <entt/entt.hpp>
#include "WorldTransform.hpp"
#include "Camera.hpp"
#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "OctreeSystem.hpp"

namespace LittleCore {

    using RenderOctreeSystem = OctreeSystem<WorldTransform, Mesh, Renderable>;

    class RenderSystem {
    public:

        RenderSystem(entt::registry &registry, RenderOctreeSystem& renderOctreeSystem);

        void Render(const WorldTransform& cameraTransform, const Camera& camera);

    private:
        entt::registry& registry;
        RenderOctreeSystem& renderOctreeSystem;
    };
}