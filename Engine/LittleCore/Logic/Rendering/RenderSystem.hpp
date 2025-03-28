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
#include "Renderer.hpp"
#include "Texturable.hpp"

namespace LittleCore {

    using RenderOctreeSystem = OctreeSystem<WorldTransform, Mesh, Renderable>;

    class RenderSystem {
    public:

        RenderSystem(entt::registry &registry);

        void Update();
        void Render(Renderer* renderer);
        void Render(bgfx::ViewId viewId, const WorldTransform& cameraTransform, const Camera& camera, Renderer* renderer);

    private:
        entt::registry& registry;
        RenderOctreeSystem renderOctreeSystem;
    };
}