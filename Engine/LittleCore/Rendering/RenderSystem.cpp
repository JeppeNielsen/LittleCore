//
// Created by Jeppe Nielsen on 21/01/2024.
//

#include "RenderSystem.hpp"
#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include <algorithm>

using namespace LittleCore;

RenderSystem::RenderSystem(entt::registry &registry, RenderOctreeSystem &renderOctreeSystem) :
registry(registry),
renderOctreeSystem(renderOctreeSystem) {

}

void RenderSystem::Render(Renderer* renderer) {
    auto view = registry.view<WorldTransform, Camera>();

    for(auto entity : view) {
        auto& worldTransform = registry.get<WorldTransform>(entity);
        auto& camera = registry.get<Camera>(entity);

        Render((bgfx::ViewId )entity, worldTransform, camera, renderer);
    }
}

void RenderSystem::Render(bgfx::ViewId viewId, const WorldTransform &cameraTransform,
                                      const Camera &camera, Renderer* renderer) {

    float width = renderer->screenSize.x * (camera.viewRect.max.x - camera.viewRect.min.x);
    float height = renderer->screenSize.y * (camera.viewRect.max.y - camera.viewRect.min.y);

    const mat4x4  projection = camera.GetProjection(width / height);

    const mat4x4 viewProjection = projection * cameraTransform.worldInverse;
    BoundingFrustum frustum;
    frustum.SetFromViewProjection(viewProjection);

    renderer->BeginRender(viewId, cameraTransform.worldInverse, projection, camera);

    std::vector<entt::entity> entities;
    renderOctreeSystem.Query(frustum, entities);

    std::sort(entities.begin(), entities.end(), [this] (entt::entity entityA, entt::entity entityB) {
        return registry.get<Renderable>(entityA).shader==registry.get<Renderable>(entityB).shader;
    });

    bgfx::ProgramHandle currentShaderProgram = BGFX_INVALID_HANDLE;

    for(auto entity : entities) {
        const Renderable& renderable = registry.get<Renderable>(entity);

        if (currentShaderProgram.idx != renderable.shader->handle.idx) {

            if (currentShaderProgram.idx != bgfx::kInvalidHandle) {
                renderer->EndBatch(viewId, currentShaderProgram);
            }

            currentShaderProgram = renderable.shader->handle;
            renderer->BeginBatch(viewId);
        }

        const WorldTransform& worldTransform = registry.get<WorldTransform>(entity);
        const Mesh& mesh = registry.get<Mesh>(entity);
        renderer->RenderMesh(mesh, worldTransform.world);
    }

    renderer->EndBatch(viewId, currentShaderProgram);
    renderer->EndRender(viewId);
}
