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

void RenderSystem::Render(uint16_t viewId, const WorldTransform &cameraTransform,
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

    std::sort(entities.begin(), entities.end(), [this, &camera, &cameraTransform](entt::entity entityA, entt::entity entityB) {

        const WorldTransform& worldTransformA = registry.get<WorldTransform>(entityA);
        const float distanceA = camera.GetDistance(cameraTransform.worldInverse, worldTransformA.world);

        const WorldTransform& worldTransformB = registry.get<WorldTransform>(entityB);
        const float distanceB = camera.GetDistance(cameraTransform.worldInverse, worldTransformB.world);

        if (std::abs(distanceA - distanceB) <= 0.01f) {
            const auto& renderableA = registry.get<Renderable>(entityA);
            const auto& renderableB = registry.get<Renderable>(entityB);
            return renderableA.shader < renderableB.shader;
        }
        return distanceA > distanceB;
    });

    const ShaderResource* prevShaderResource = nullptr;
    const ShaderResource* currentShaderResource = nullptr;

    BlendMode currentBlendMode = BlendMode::Off;
    BlendMode prevBlendMode = BlendMode::Off;

    uint64_t currentHash = 0;
    uint64_t previousHash = 0;
    entt::entity currentRenderable = entt::null;
    entt::entity prevRenderable = entt::null;

    //std::cout << "Render started\n";

    bool startedBatch = false;
    for (int i = 0; i < entities.size(); ++i) {
        auto entity = entities[i];

        const WorldTransform& worldTransformA = registry.get<WorldTransform>(entity);
        const float distanceA = camera.GetDistance(cameraTransform.worldInverse, worldTransformA.world);
       // std::cout << std::to_string((int)entity) << " -> " << distanceA << "\n";

        const Mesh* mesh = registry.get<Mesh>(entity)->operator->();
        if (mesh->vertices.empty() || mesh->triangles.empty()) {
            continue;
        }

        const Renderable& renderable = registry.get<Renderable>(entity);
        const Texturable* texturable = registry.try_get<Texturable>(entity);

        currentShaderResource = renderable.shader ? renderable.shader.operator->() : nullptr;
        currentBlendMode = renderable.blendMode;

        currentHash = renderable.uniforms.CalculateHash();

        currentRenderable = entity;

        if (!startedBatch) {
            renderer->BeginBatch(viewId);
            prevShaderResource = currentShaderResource;
            prevBlendMode = currentBlendMode;
            previousHash = currentHash;
            prevRenderable = currentRenderable;
            startedBatch = true;
        } else {

            const sg_shader currentShaderHandle = currentShaderResource ? currentShaderResource->handle : sg_shader{SG_INVALID_ID};
            const sg_shader prevShaderHandle = prevShaderResource ? prevShaderResource->handle : sg_shader{SG_INVALID_ID};
            if (currentShaderHandle.id != prevShaderHandle.id ||
                currentBlendMode != prevBlendMode ||
                currentHash != previousHash) {
                renderer->SetUniforms(registry.get<Renderable>(prevRenderable).uniforms);
                renderer->EndBatch(viewId, prevShaderResource, prevBlendMode);
                prevShaderResource = currentShaderResource;
                prevBlendMode = currentBlendMode;
                previousHash = currentHash;
                prevRenderable = currentRenderable;
                renderer->BeginBatch(viewId);
            }
        }

        const WorldTransform& worldTransform = registry.get<WorldTransform>(entity);
        renderer->RenderMesh(*mesh, worldTransform.world);
    }

    if (currentRenderable!=entt::null) {
        renderer->SetUniforms(registry.get<Renderable>(currentRenderable).uniforms);
    }
    renderer->EndBatch(viewId, currentShaderResource, currentBlendMode);
    renderer->EndRender(viewId);

    std::cout << "\n";
}

void RenderSystem::Update() {
    renderOctreeSystem.Update();
}
