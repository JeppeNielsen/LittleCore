//
// Created by Jeppe Nielsen on 21/01/2024.
//
#include "gtest/gtest.h"
#include "RenderSystem.hpp"

using namespace LittleCore;

namespace {
    TEST(RenderSystem, RenderSystemTests) {

        entt::registry registry;

        RenderOctreeSystem renderOctreeSystem(registry);

        RenderSystem renderSystem(registry, renderOctreeSystem);

        auto entity = registry.create();

        auto& mesh = registry.emplace<Mesh>(entity);

        mesh.vertices.push_back({{-1,-1,0},0, {0,0}});
        mesh.vertices.push_back({{1,-1,0},0, {0,0}});
        mesh.vertices.push_back({{1,1,0},0, {0,0}});
        mesh.vertices.push_back({{-1,1,0},0, {0,0}});

        mesh.triangles.push_back(0);
        mesh.triangles.push_back(1);
        mesh.triangles.push_back(2);
        mesh.triangles.push_back(0);
        mesh.triangles.push_back(2);
        mesh.triangles.push_back(3);

        auto& worldTransform = registry.emplace<WorldTransform>(entity);
        worldTransform.world = glm::identity<mat4x4>();
        auto& renderable = registry.emplace<Renderable>(entity);
        auto& worldBoundingBox = registry.emplace<WorldBoundingBox>(entity);

        worldBoundingBox.bounds = {{0,0,0} , {10,10,10}};

        registry.patch<WorldBoundingBox>(entity);

        struct TestRenderer : public Renderer {
            int renderCount = 0;

            void BeginRender(bgfx::ViewId viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) override {

            }

            void EndRender(bgfx::ViewId viewId) override {

            }

            void BeginBatch(bgfx::ViewId viewId) override {

            }

            void RenderMesh(const Mesh& mesh, const glm::mat4x4& world) override {
                renderCount++;
            }

            void EndBatch(bgfx::ViewId viewId, bgfx::ProgramHandle shaderProgram) override {

            }
        };

        TestRenderer testRenderer;

        WorldTransform cameraTransform;
        cameraTransform.world = glm::translate(glm::identity<mat4x4>(), {0,0,-10});
        cameraTransform.worldInverse = glm::inverse(cameraTransform.world);
        Camera camera;
        camera.near = 1;
        camera.far = 200;
        camera.viewRect = {{-10,-10},{10,10}};

        const mat4x4 viewProjection = camera.GetProjection(1.0f) * cameraTransform.worldInverse;
        BoundingFrustum frustum;
        frustum.SetFromViewProjection(viewProjection);

        EXPECT_TRUE(frustum.Intersect(worldBoundingBox.bounds)!=LittleCore::BoundingFrustum::OUTSIDE);

        renderOctreeSystem.Update();
        renderSystem.Render(0, cameraTransform, camera, &testRenderer);

        EXPECT_EQ(1, testRenderer.renderCount);
    }

}