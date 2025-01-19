//
// Created by Jeppe Nielsen on 21/01/2024.
//

#include "RenderSystem.hpp"
#include "WorldTransform.hpp"
#include "Mesh.hpp"
#include "Renderable.hpp"
#include "Texturable.hpp"
#include <algorithm>
#include <iostream>

using namespace LittleCore;

RenderSystem::RenderSystem(entt::registry &registry) :
registry(registry),
renderOctreeSystem(registry) {
}

void RenderSystem::Render(Renderer* renderer) {
    auto view = registry.view<WorldTransform, Camera>();

    for(auto entity : view) {
        auto& worldTransform = registry.get<WorldTransform>(entity);
        auto& camera = registry.get<Camera>(entity);

        Render(0, worldTransform, camera, renderer);
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
    bgfx::TextureHandle currentTexture = BGFX_INVALID_HANDLE;
    auto colorTextureUniform = uniforms.GetHandle("colorTexture", bgfx::UniformType::Sampler);

    for(auto entity : entities) {
        const Renderable& renderable = registry.get<Renderable>(entity);
        Texturable* texturable = registry.try_get<Texturable>(entity);

        renderer->BeginBatch(viewId);

        if (texturable) {
            bgfx::setTexture(0, colorTextureUniform, texturable->texture->handle);
        }

        const WorldTransform& worldTransform = registry.get<WorldTransform>(entity);
        const Mesh& mesh = registry.get<Mesh>(entity);
        renderer->RenderMesh(mesh, worldTransform.world);
        renderer->EndBatch(viewId, renderable.shader->handle);
    }

    std::cout << "Num entities rendered : " << entities.size()<<"\n";

    renderer->EndRender(viewId);

}

void RenderSystem::Update() {
    renderOctreeSystem.Update();
}
