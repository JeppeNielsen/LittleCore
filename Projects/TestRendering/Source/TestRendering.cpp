//
// Created by Jeppe Nielsen on 08/08/2024.
//

#include "TestRendering.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
/*
struct MovementKey {
    InputKey key;
    vec3 direction;
    bool isActive;
};

struct Movable {
    std::vector<MovementKey> keys;
};
 */

entt::entity CreateQuadNew(entt::registry& registry, glm::vec3 position, entt::entity parent = entt::null) {

    auto quad = registry.create();
    registry.emplace<LocalTransform>(quad).position = position;
    registry.emplace<WorldTransform>(quad);
    registry.emplace<Hierarchy>(quad).parent = parent;
    auto& mesh = registry.emplace<Mesh>(quad);
    mesh.vertices.push_back({{-1,-1,0}, 0x00FF0000 , {0,0}});
    mesh.vertices.push_back({{1,-1,0}, 0xff000000 , {0,1} });
    mesh.vertices.push_back({{1,1,0}, 0xff000000, {1,1}});
    mesh.vertices.push_back({{-1,1,0}, 0xff000000,{1,0}});
    mesh.triangles.push_back(0);
    mesh.triangles.push_back(1);
    mesh.triangles.push_back(2);

    mesh.triangles.push_back(0);
    mesh.triangles.push_back(2);
    mesh.triangles.push_back(3);

    registry.emplace<Renderable>(quad);
    registry.emplace<LocalBoundingBox>(quad);
    registry.emplace<WorldBoundingBox>(quad);

    return quad;
}

void TestRendering::Initialize() {

    imGuiController.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Game");
        ImTextureID textureId = (ImTextureID)(uintptr_t)renderTexture.idx;
        ImGui::Image(textureId, ImVec2((float)renderTextureWidth, (float)renderTextureHeight));

        ImGui::End();

        ImGui::Begin("Scene");
        ImTextureID textureId2 = (ImTextureID)(uintptr_t)renderTexture.idx;
        ImGui::Image(textureId2, ImVec2((float)renderTextureWidth, (float)renderTextureHeight));

        ImGui::End();
    });

    resourcePathMapper.RefreshFromRootPath("../../../../Assets/");
    resources.CreateLoaderFactory<TextureResourceLoaderFactory>();
    resources.CreateLoaderFactory<ShaderResourceLoaderFactory>();


    {
        auto cameraObject = registry.create();
        registry.emplace<LocalTransform>(cameraObject).position = {0, 0, -10};
        registry.emplace<WorldTransform>(cameraObject);
        registry.emplace<Hierarchy>(cameraObject);
        /*auto& moveable = registry.emplace<Movable>(cameraObject);
        moveable.keys.push_back({
                                        InputKey::A,
                                        {-1,0,0}
                                });

        moveable.keys.push_back({
                                        InputKey::D,
                                        {1,0,0}
                                });

        moveable.keys.push_back({
                                        InputKey::W,
                                        {0,0,1}
                                });

        moveable.keys.push_back({
                                        InputKey::S,
                                        {0,0,-1}
                                });
                                */

        registry.emplace<Input>(cameraObject);

        auto &camera = registry.emplace<Camera>(cameraObject);
        camera.fieldOfView = 60.0f;
        camera.near = 1;
        camera.far = 20;
        camera.viewRect = {{0,    0},
                           {1.0f, 1.0f}};
        cameraEntity = cameraObject;
    }


    shader = resources.Create<ShaderResource>("4EBD82BDCBCA4F78B597C8B2DF9A08F7");
    texture = resources.Create<TextureResource>("B62D424BF40F46359248CDE498930422");

    colorTexture  = bgfx::createUniform("colorTexture",  bgfx::UniformType::Sampler);

    auto quad1 = CreateQuadNew(registry, {0, 0, 0});
    registry.get<Renderable>(quad1).shader = shader;

    auto quad2 = CreateQuadNew(registry, {3, 0, 0}, quad1);
    registry.get<Renderable>(quad2).shader = shader;

    auto quad3 = CreateQuadNew(registry, {0, 2, 0}, quad2);
    registry.get<Renderable>(quad3).shader = shader;

    int width;
    int height;
    SDL_GetWindowSizeInPixels((SDL_Window*)mainWindow, &width, &height);
    bgfxRenderer.screenSize = {width, height};


    renderTexture = bgfx::createTexture2D(
            renderTextureWidth, renderTextureHeight,
            false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT
    );

    framebuffer = bgfx::createFrameBuffer(1, &renderTexture, true);

}

void TestRendering::Update(float dt) {
    //registry.patch<LocalTransform>(quad2).position = {0, 4 + sinf(time) * 4, 0};
    //registry.patch<LocalTransform>(quad1).position = {6 + sinf(time)*6,0,0};
    //registry.patch<LocalTransform>(quad1).rotation =  glm::quat({0,0,scale});

    time += dt;

    registry.patch<LocalTransform>(cameraEntity).rotation = glm::quat({0, 0, time});
    registry.patch<Camera>(cameraEntity).fieldOfView = 20;

    simulation.Update();
}

void TestRendering::Render() {

    bgfx::setViewFrameBuffer(0, framebuffer);
    bgfx::setViewRect(0, 0, 0, renderTextureWidth, renderTextureHeight);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    bgfx::touch(0);

    if (fmodf(time, 1.0f)<0.5f) {
        bgfx::setTexture(0, colorTexture, texture->handle);
    }
    simulation.Render(bgfxRenderer);
    bgfx::frame();

    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);

    imGuiController.Render();

}

TestRendering::TestRendering() : simulation(registry), resources(resourcePathMapper) {

}

void TestRendering::HandleEvent(void *event) {
    imGuiController.HandleEvent(event);
}
