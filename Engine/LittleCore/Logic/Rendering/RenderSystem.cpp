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

    const mat4x4 projection = camera.GetProjection(width / height);

    const mat4x4 viewProjection = projection * cameraTransform.worldInverse;
    BoundingFrustum frustum;
    frustum.SetFromViewProjection(viewProjection);

    renderer->BeginRender(viewId, cameraTransform.worldInverse, projection, camera);

    std::vector<entt::entity> entities;
    renderOctreeSystem.Query(frustum, entities);

    std::sort(entities.begin(), entities.end(), [this](entt::entity entityA, entt::entity entityB) {
        const auto& renderableA = registry.get<Renderable>(entityA);
        const auto& renderableB = registry.get<Renderable>(entityB);
        return renderableA.shader < renderableB.shader;
    });

    bgfx::ProgramHandle prevShader = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle prevTexture = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle currentShader = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle currentTexture = BGFX_INVALID_HANDLE;
    BlendMode currentBlendMode = BlendMode::Off;
    BlendMode prevBlendMode = BlendMode::Off;


    bool startedBatch = false;
    for (int i = 0; i < entities.size(); ++i) {
        auto entity = entities[i];

        const Mesh* mesh = registry.get<Mesh>(entity)->operator->();
        if (mesh->vertices.empty() || mesh->triangles.empty()) {
            continue;
        }

        const Renderable& renderable = registry.get<Renderable>(entity);
        const Texturable* texturable = registry.try_get<Texturable>(entity);

        currentShader = renderable.shader ? (bgfx::ProgramHandle) renderable.shader->handle
                                          : (bgfx::ProgramHandle) BGFX_INVALID_HANDLE;
        currentTexture = (texturable) ? (bgfx::TextureHandle) (texturable->operator->()->texture)
                                      : (bgfx::TextureHandle) BGFX_INVALID_HANDLE;
        currentBlendMode = renderable.blendMode;

        if (!startedBatch) {
            renderer->BeginBatch(viewId);
            prevShader = currentShader;
            prevTexture = currentTexture;
            prevBlendMode = currentBlendMode;
            startedBatch = true;
        } else {

            if (currentTexture.idx != prevTexture.idx ||
                currentShader.idx != prevShader.idx ||
                currentBlendMode != prevBlendMode) {
                renderer->SetTexture("colorTexture", prevTexture);
                renderer->EndBatch(viewId, prevShader, prevBlendMode);
                prevTexture = currentTexture;
                prevShader = currentShader;
                prevBlendMode = currentBlendMode;
                renderer->BeginBatch(viewId);
            }
        }

        const WorldTransform& worldTransform = registry.get<WorldTransform>(entity);
        renderer->RenderMesh(*mesh, worldTransform.world);
    }

    renderer->SetTexture("colorTexture", currentTexture);
    renderer->EndBatch(viewId, currentShader, currentBlendMode);

    renderer->EndRender(viewId);
}

void RenderSystem::Update() {
    renderOctreeSystem.Update();
}
