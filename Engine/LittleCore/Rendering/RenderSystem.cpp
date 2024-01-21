//
// Created by Jeppe Nielsen on 21/01/2024.
//

#include "RenderSystem.hpp"
#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"


LittleCore::RenderSystem::RenderSystem(entt::registry &registry, LittleCore::RenderOctreeSystem &renderOctreeSystem) :
        registry(registry),
        renderOctreeSystem(renderOctreeSystem)
{

}

void LittleCore::RenderSystem::Render(const LittleCore::WorldTransform &cameraTransform, const LittleCore::Camera& camera) {


    const mat4x4 viewProjection = camera.GetProjection() * cameraTransform.worldInverse;
    BoundingFrustum frustum;
    frustum.SetFromViewProjection(viewProjection);

    std::vector<entt::entity> entities;
    renderOctreeSystem.Query(frustum, entities);



}
