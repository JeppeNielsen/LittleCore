//
// Created by Jeppe Nielsen on 13/04/2024.
//
#include "Engine.hpp"
#include "ImGuiController.hpp"
#include <iostream>
#include "NetimguiClient.hpp"
#include <thread>
#include <SDL3/SDL.h>
#include <entt/entt.hpp>
#include "DefaultSimulation.hpp"
#include "SDLInputHandler.hpp"
#include "BgfxRenderer.hpp"
#include "DefaultResourceManager.hpp"

using namespace LittleCore;

struct Rotatable {
    float speed = 0.01f;

};

struct RotationSystem  {

    entt::registry& registry;

    RotationSystem(entt::registry& registry) : registry(registry) {}


    void Update() {

        auto view = registry.view<const Rotatable, LocalTransform>();
        for(auto[entity, rotatable, transform] : view.each()) {
            auto roll = glm::roll(transform.rotation);
            transform.rotation = glm::quat({0,0,roll + rotatable.speed});
            registry.patch<LocalTransform>(entity);
        }
    }

};

struct TestNetimguiClient : IState {
    ImGuiController gui;
    NetimguiClient netimguiClient;

    entt::registry registry;
    CustomSimulation<RotationSystem> simulation;
    SDLInputHandler sdlInputHandler;
    BGFXRenderer renderer;
    ResourcePathMapper resourcePathMapper;
    DefaultResourceManager resourceManager;


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

        netimguiClient.Start();
        netimguiClient.Connect("Test client", "localhost");

        while (netimguiClient.IsConnectionPending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds (16));
        }
        if (!netimguiClient.IsConnected()) {
            std::cout << "couldn't connect\n";
        }

        resourcePathMapper.RefreshFromRootPath("/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source");
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

        auto quad1 = CreateQuadNew(registry, {0, 0, 0}, {1,1,1});
        registry.emplace<Rotatable>(quad1);

        auto child = CreateQuadNew(registry, {1,1,-0.4}, vec3(1,1,1) * 0.5f, quad1);

        int width;
        int height;
        SDL_GetWindowSizeInPixels((SDL_Window*)mainWindow, &width, &height);
        renderer.screenSize = {width, height};

        /*
        auto cameraEntity = registry.create();
        registry.emplace<WorldTransform>(cameraEntity);
        registry.emplace<LocalTransform>(cameraEntity).position = {0,0,10};
        registry.emplace<Camera>(cameraEntity);
        registry.emplace<Hierarchy>(cameraEntity);
*/

        /*
        auto quadEntity = registry.create();
        registry.emplace<WorldTransform>(quadEntity);
        registry.emplace<LocalTransform>(quadEntity);
        registry.emplace<Hierarchy>(quadEntity);

        Mesh& mesh = registry.emplace<Mesh>(quadEntity);
        mesh.vertices.push_back({{0,0,0}});
        mesh.vertices.push_back({{1,0,0}});
        mesh.vertices.push_back({{1,1,0}});
        mesh.vertices.push_back({{0,1,0}});

        mesh.triangles.push_back(0);
        mesh.triangles.push_back(1);
        mesh.triangles.push_back(2);

        mesh.triangles.push_back(0);
        mesh.triangles.push_back(3);
        mesh.triangles.push_back(2);

        registry.emplace<LocalBoundingBox>(quadEntity);
        registry.emplace<WorldBoundingBox>(quadEntity);
        registry.emplace<Renderable>(quadEntity).shader = resourceManager.Create<ShaderResource>("65886F92DEC94836A9E2FEA6C3483543");

         */

    }

    void HandleEvent(void* event) override {
        //gui.HandleEvent(event);
        simulation.HandleEvent(event, sdlInputHandler);
    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("My test window");

        if (ImGui::Button("Close")) {
            abort();
        }

        ImGui::End();
    }

    void Update(float dt) override {
        simulation.Update();
    }

    void Render() override {

        simulation.Render(renderer);
        bgfx::touch(0);

        //gui.Render();

    }
};

int main() {
    Engine e({"Netimgui Client", true});
    e.Start<TestNetimguiClient>();
    return 0;
}

