//
// Created by Jeppe Nielsen on 08/08/2024.
//

#include "TestRendering.hpp"
#include <sokol_app.h>
#include "imgui.h"
#include <cstdint>
#include "Movable.hpp"
#include "DefaultSimulation.hpp"
#include "Texturable.hpp"
#include <iostream>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include <util/sokol_imgui.h>

entt::entity CreateQuadNew(entt::registry& registry, glm::vec3 position, glm::vec3 scale, entt::entity parent = entt::null) {

    auto quad = registry.create();
    auto& tr = registry.emplace<LocalTransform>(quad);
    tr.position = position;
    tr.scale = scale;
    registry.emplace<WorldTransform>(quad);
    registry.emplace<Hierarchy>(quad).parent = parent;
    auto& mesh = registry.emplace<Mesh>(quad);
    mesh.vertices.push_back({{-1,-1,0}, 0xFFFFFF , {0,0}});
    mesh.vertices.push_back({{1,-1,0}, 0xFFFFFF , {0,1} });
    mesh.vertices.push_back({{1,1,0}, 0xFFFFFF, {1,1}});
    mesh.vertices.push_back({{-1,1,0}, 0xFFFFFF,{1,0}});
    mesh.triangles.push_back(0);
    mesh.triangles.push_back(1);
    mesh.triangles.push_back(2);

    mesh.triangles.push_back(0);
    mesh.triangles.push_back(2);
    mesh.triangles.push_back(3);

    registry.emplace<Renderable>(quad);
    registry.emplace<LocalBoundingBox>(quad);
    registry.emplace<WorldBoundingBox>(quad);
    registry.emplace<Texturable>(quad);

    return quad;
}

struct Settings {
    int value;
};

void TestRendering::Initialize() {
    DefaultSimulation sim;
    sim.Update(0.0f);
    auto& registry = simulation.registry;

    imGuiController.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Game");
        if (renderTexture.id != SG_INVALID_ID) {
            ImTextureID textureId = static_cast<ImTextureID>(simgui_imtextureid(renderTexture));
            ImGui::Image(textureId, ImVec2((float)renderTextureWidth, (float)renderTextureHeight));
        }

        ImGui::End();

        ImGui::Begin("Scene 2");
        if (renderTexture.id != SG_INVALID_ID) {
            ImTextureID textureId2 = static_cast<ImTextureID>(simgui_imtextureid(renderTexture));
            for (int i = 0; i < 10; ++i) {
                ImGui::Image(textureId2, ImVec2((float)256, (float)256));
            }
        }

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
        auto& moveable = registry.emplace<Movable>(cameraObject);
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
    texture = resources.Create<Texturable>("B62D424BF40F46359248CDE498930422");
    texture2 = resources.Create<Texturable>("0C73A4153FCA4854A1CFBB8BCC33E1AC");

   quad1 = CreateQuadNew(registry, {0, 0, 0}, {1,1,1});
   registry.get<Renderable>(quad1).shader = shader;
   registry.get<Texturable>(quad1).handle = texture;

    quad2 = CreateQuadNew(registry, {3, 0, 0}, {1,0.05,1});
    registry.get<Renderable>(quad2).shader = shader;
    registry.get<Texturable>(quad2).handle = texture2;

    auto quad3 = CreateQuadNew(registry, {3, 2, 0}, {0.05, 1,1});
    registry.get<Renderable>(quad3).shader = shader;
    registry.get<Texturable>(quad3).handle = texture;


    auto& quadMovable = registry.emplace<Movable>(quad1);
    quadMovable.keys.push_back({
                                    InputKey::F,
                                    {-1,0,0}
                            });

    quadMovable.keys.push_back({
                                    InputKey::H,
                                    {1,0,0}
                            });

    quadMovable.keys.push_back({
                                    InputKey::T,
                                    {0,1,0}
                            });

    quadMovable.keys.push_back({
                                    InputKey::G,
                                    {0,-1,0}
                            });
    registry.emplace<Input>(quad1);


    renderer.screenSize = {sapp_widthf(), sapp_heightf()};


    sg_image_desc imageDesc{};
    imageDesc.render_target = true;
    imageDesc.width = renderTextureWidth;
    imageDesc.height = renderTextureHeight;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    renderTexture = sg_make_image(imageDesc);

    sg_attachments_desc attachmentsDesc{};
    attachmentsDesc.colors[0].image = renderTexture;
    framebuffer = sg_make_attachments(attachmentsDesc);

}

void TestRendering::Update(float dt) {
   // registry.patch<LocalTransform>(quad2).position = {0, -1 + sinf(time*0.5f) *1, 0};
    //registry.patch<LocalTransform>(quad1).position = {2 + sinf(time)*2,0,0};

    time += dt;

    //registry.patch<LocalTransform>(cameraEntity).rotation = glm::quat({0, 0, time});
    //registry.patch<Camera>(cameraEntity).fieldOfView = 40;


    simulation.Update(dt);
}

void TestRendering::Render() {

    sg_pass_action passAction{};
    passAction.colors[0].load_action = SG_LOADACTION_CLEAR;
    passAction.colors[0].store_action = SG_STOREACTION_STORE;
    passAction.colors[0].clear_value = {0.188f, 0.188f, 0.188f, 1.0f};
    passAction.depth.load_action = SG_LOADACTION_CLEAR;
    passAction.depth.store_action = SG_STOREACTION_DONTCARE;
    passAction.depth.clear_value = 1.0f;

    sg_pass pass{};
    pass.action = passAction;
    pass.attachments = framebuffer;
    sg_begin_pass(pass);

    simulation.Render(renderer);
    sg_end_pass();

    imGuiController.Render();
}

TestRendering::TestRendering() : resources(resourcePathMapper) {

}

void TestRendering::HandleEvent(void *event) {
    imGuiController.HandleEvent(event);
    simulation.HandleEvent(event, inputHandler);
}
