//
// Created by Jeppe Nielsen on 21/01/2024.
//

#include "RenderSystem.hpp"
#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include <algorithm>

LittleCore::RenderSystem::RenderSystem(entt::registry &registry, LittleCore::RenderOctreeSystem &renderOctreeSystem) :
registry(registry),
renderOctreeSystem(renderOctreeSystem)
{

}

void LittleCore::RenderSystem::Render(bgfx::ViewId viewId, const LittleCore::WorldTransform &cameraTransform,
                                      const LittleCore::Camera &camera, LittleCore::Renderer *renderer) {

    const mat4x4  projection = camera.GetProjection(1.0f);

    const mat4x4 viewProjection = projection * cameraTransform.world;
    BoundingFrustum frustum;
    frustum.SetFromViewProjection(viewProjection);

    renderer->BeginRender(viewId, cameraTransform.worldInverse, projection);



    std::vector<entt::entity> entities;
    //renderOctreeSystem.Query(frustum, entities);

    auto view = registry.view<WorldTransform, Mesh, Renderable>();
    for(auto e : view) {
        entities.push_back(e);
    }


    std::sort(entities.begin(), entities.end(), [this] (entt::entity entityA, entt::entity entityB) {
        return registry.get<Renderable>(entityA).shaderProgram.idx==registry.get<Renderable>(entityB).shaderProgram.idx;
    });

    bgfx::ProgramHandle currentShaderProgram = BGFX_INVALID_HANDLE;


    for(auto entity : entities) {
        const Renderable& renderable = registry.get<Renderable>(entity);

        if (currentShaderProgram.idx != renderable.shaderProgram.idx) {

            if (currentShaderProgram.idx != bgfx::kInvalidHandle) {
                renderer->EndBatch(viewId, currentShaderProgram);
            }

            currentShaderProgram = renderable.shaderProgram;
            renderer->BeginBatch(viewId);
        }

        const WorldTransform& worldTransform = registry.get<WorldTransform>(entity);
        const Mesh& mesh = registry.get<Mesh>(entity);
        renderer->RenderMesh(mesh, worldTransform.world);
    }

    renderer->EndBatch(viewId, currentShaderProgram);
    renderer->EndRender(viewId);
}
