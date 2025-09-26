//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include <iostream>
#include "NetimguiClientController.hpp"
#include <thread>
#include <SDL3/SDL.h>
#include "StorageFactory.hpp"
#include "DefaultSimulation.hpp"
#include "SDLInputHandler.hpp"
#include "BgfxRenderer.hpp"
#include "DefaultResourceManager.hpp"
#include "ObjectGuiDrawer.hpp"
#include <glm/glm.hpp>
#include "ResizableFrameBuffer.hpp"
#include "MathReflection.hpp"
#include "EntityGuiDrawer.hpp"

using namespace LittleCore;


struct Rotatable {
    float speed = 0.0f;
    float speedY = 0.0f;
    float speedX = 0;
};

struct Player {
    void Start() {
        age++;
    }
    std::string name;
    int age = 12;
    float percentage = 0.5f;
    glm::vec3 position = {2,1,0};
    glm::vec2 size = {1,1};
    glm::quat rotation = glm::quat({0,0,0});
    LittleCore::Rect viewPort;
};

struct RotationSystem : UpdateSystem {

    void Update(float dt) {

        auto view = registry.view<const Rotatable, LocalTransform>();
        for(auto[entity, rotatable, transform] : view.each()) {
            auto roll = glm::roll(transform.rotation);
            auto yaw = glm::yaw(transform.rotation);

            transform.rotation = glm::quat({0, yaw + rotatable.speedY * dt,roll + rotatable.speed * dt});
            registry.patch<LocalTransform>(entity);
        }
    }

};

struct Wobbler {
    float speed = 0;
    vec3 direction = vec3(0,1,0);
    float position = 0;
};



struct WobblerSystem : UpdateSystem {

    void Update(float dt) {
        auto view = registry.view<LocalTransform, Wobbler>();
        for(auto[entity, transform, wobbler] : view.each()) {
            transform.position = vec3(0,0,0) + wobbler.direction * sin(wobbler.position);
            wobbler.position += wobbler.speed * dt;
        }
    }

};


struct TestNetimguiClient : IState {
    ImGuiController gui;
    NetimguiClientController netimguiClientController;

    entt::registry registry;
    CustomSimulation<RotationSystem, WobblerSystem> simulation;
    SDLInputHandler sdlInputHandler;
    BGFXRenderer renderer;
    ResourcePathMapper resourcePathMapper;
    DefaultResourceManager resourceManager;
    ResizableFrameBuffer frameBuffer;
    ImVec2 gameSize;
    EntityGuiDrawer drawer;

    Player player;
    entt::entity quad;
    entt::entity child;

    TestNetimguiClient() : simulation(registry), resourceManager(resourcePathMapper) {}

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

        registry.get<Renderable>(quad).shader = resourceManager.Create<ShaderResource>("65886F92DEC94836A9E2FEA6C3483543");
        registry.get<Texturable>(quad).texture = resourceManager.Create<TextureResource>("020CDC91085545C0AB724CC5C3645F3B");


        return quad;
    }


    void Initialize() override {

        gui.Initialize(mainWindow, [this]() {
            OnGUI();
        });

        gui.LoadFont("/Users/jeppe/Jeppes/LittleCore/Projects/TestImGui/Source/Fonts/LucidaG.ttf", 12);

        netimguiClientController.Start();
        netimguiClientController.Connect("Test client", "localhost");

        while (netimguiClientController.IsConnectionPending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds (16));
        }
        if (!netimguiClientController.IsConnected()) {
            std::cout << "couldn't connect\n";
        }

        resourcePathMapper.RefreshFromRootPath("/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/");
        resourceManager.CreateLoaderFactory<ShaderResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<TextureResourceLoaderFactory>();

        {
            auto cameraObject = registry.create();
            registry.emplace<LocalTransform>(cameraObject).position = {0, 0, -10};
            registry.emplace<WorldTransform>(cameraObject);
            registry.emplace<Hierarchy>(cameraObject);

            auto &camera = registry.emplace<Camera>(cameraObject);
            camera.fieldOfView = 30.0f;
            camera.near = 1;
            camera.far = 20;
            camera.viewRect = {{0,    0},
                               {1.0f, 1.0f}};
        }

        quad = CreateQuadNew(registry, {0, 0, 0}, {1,1,1});
        registry.emplace<Rotatable>(quad);

        child = CreateQuadNew(registry, {1,1,-0.4}, vec3(1,1,1) * 0.5f, quad);
        registry.emplace<Rotatable>(child);

        registry.emplace<Wobbler>(quad);

        std::string playerJson;
        auto error = glz::write<glz::opts{.prettify = true}>(player, playerJson);

        std::cout << playerJson << "\n";
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
        simulation.HandleEvent(event, sdlInputHandler);
    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Hierarchy");

        if (ImGui::Button("Close")) {
            abort();
        }
        ImGui::End();
        ImGui::Begin("Game");

        gameSize = ImGui::GetContentRegionAvail();

        ImGui::Image((void*)(uintptr_t)(frameBuffer.texture.idx), gameSize);

        ImGui::End();

        ImGui::Begin("Inspector");
        auto &storage = registry.storage<entt::entity>();
        for (auto entity : storage) {
            if (registry.valid(entity)) {
                DrawEntity(entity);
            }
        }

        ImGui::End();
    }

    void DrawEntity(entt::entity e) {

        ImGui::PushID((int)e);

        GuiHelper::DrawHeader("Entity:");
        drawer.Draw(registry, e);

        ImGui::PopID();
    }

    void Update(float dt) override {
        simulation.Update(dt);

        renderer.screenSize = {gameSize.x, gameSize.y};
        if (gameSize.x>32 && gameSize.y>32) {
            frameBuffer.Render((int) gameSize.x, (int) gameSize.y, [this]() {
                simulation.Render(renderer);
                bgfx::touch(0);
                bgfx::frame();
            });
            netimguiClientController.SendTexture(frameBuffer.texture,  frameBuffer.width, frameBuffer.height);
        }
    }

    void Render() override {
        gui.Render();
    }
};

int main() {
    Engine e({"Netimgui Client", true});
    e.Start<TestNetimguiClient>();
    return 0;
}

