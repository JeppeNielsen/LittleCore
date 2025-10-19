//
// Created by Jeppe Nielsen on 13/04/2024.
//

#include "Engine.hpp"
#include "EditorSimulations/EditorSimulationRegistry.hpp"
#include "ImGuiController.hpp"
#include <iostream>
#include "NetimguiClientController.hpp"
#include <thread>
#include <SDL3/SDL.h>
#include "DefaultSimulation.hpp"
#include "SDLInputHandler.hpp"
#include "BgfxRenderer.hpp"
#include "DefaultResourceManager.hpp"
#include <glm/glm.hpp>
#include "MathReflection.hpp"
#include "EntityGuiDrawer.hpp"
#include "SystemBase.hpp"
#include "InputRotationSystem.hpp"
#include "MovableSystem.hpp"
#include "PickingSystem.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "GuiResourceDrawers.hpp"
#include "MeshLoader.hpp"

using namespace LittleCore;

struct Rotatable {
    float speed = 0.0f;
    float speedY = 0.0f;
    float speedX = 0;
    float yaw =0;
};

struct RotationSystem : SystemBase {

    void Update(float dt) {

        auto view = registry.view<Rotatable, LocalTransform>();
        for(auto[entity, rotatable, transform] : view.each()) {
            //auto roll = glm::roll(transform.rotation);
            //auto yaw = glm::yaw(transform.rotation);
            rotatable.yaw += dt * rotatable.speedY;

            transform.rotation = glm::quat({0, rotatable.yaw,0});
            registry.patch<LocalTransform>(entity);
        }
    }

};

struct Wobbler {
    float speed = 0;
    vec3 direction = vec3(0,1,0);
    float position = 0;
};

struct WobblerSystem : SystemBase {

    void Update(float dt) {
        auto view = registry.view<LocalTransform, Wobbler>();
        for(auto[entity, transform, wobbler] : view.each()) {
            transform.position = vec3(0,0,0) + wobbler.direction * sin(wobbler.position);
            wobbler.position += wobbler.speed * dt;
            registry.patch<LocalTransform>(entity);
        }
    }
};

struct TestNetimguiClient : IState {
    BGFXRenderer renderer;
    EditorSimulationContext editorSimulationContext;
    EditorSimulationRegistry editorSimulationRegistry;
    NetimguiClientController netimguiClientController;
    ImGuiController gui;
    Project project;
    ProjectWindow projectWindow;

    CustomSimulation<RotationSystem, WobblerSystem, MovableSystem, InputRotationSystem> simulation;
    SDLInputHandler sdlInputHandler;

    DefaultResourceManager resourceManager;
    EntityGuiDrawerContext drawerContext;
    EntityGuiDrawer<LocalTransform, Wobbler, Camera, Rotatable, Texturable, Renderable, Mesh> drawer;

    TestNetimguiClient() :
            drawerContext(resourceManager),
            drawer(drawerContext),
    editorSimulationContext(renderer, netimguiClientController, drawer),
    editorSimulationRegistry(editorSimulationContext),
    resourceManager(project.resourcePathMapper) {}

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
        mesh.vertices.push_back({{-1,1,0}, 0x00FFFF,{1,0}});
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

        editorSimulationRegistry.AddSimulation(simulation);

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

        project.rootPath = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/";
        project.resourcePathMapper.RefreshFromRootPath(project.rootPath);

        resourceManager.CreateLoaderFactory<ShaderResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<TextureResourceLoaderFactory>();
        resourceManager.CreateLoaderFactory<MeshResourceLoaderFactory>();

        auto& registry = simulation.registry;
        {
            auto cameraObject = registry.create();
            registry.emplace<LocalTransform>(cameraObject).position = {0, 0, -10};
            registry.emplace<WorldTransform>(cameraObject);
            registry.emplace<Hierarchy>(cameraObject);

            auto &camera = registry.emplace<Camera>(cameraObject);
            camera.fieldOfView = 30.0f;
            camera.near = 1;
            camera.far = 200;
            camera.viewRect = {{0,    0},
                               {1.0f, 1.0f}};


            auto& movable = registry.emplace<Movable>(cameraObject);
            movable.speed = 10.0f;
            movable.keys.push_back({
                InputKey::A,
                vec3(-1,0,0)
            });
            movable.keys.push_back({
                                                    InputKey::D,
                                                    vec3(1,0,0)
                                            });
            movable.keys.push_back({
                                                    InputKey::W,
                                                    vec3(0,0,1)
                                            });
            movable.keys.push_back({
                                                    InputKey::S,
                                                    vec3(0,0,-1)
                                            });
            registry.emplace<Input>(cameraObject);

            auto& inputRotation = registry.emplace<InputRotation>(cameraObject);

        }

        auto quad = CreateQuadNew(registry, {0, 0, 0}, {1,1,1});
        /*auto child = CreateQuadNew(registry, {1,1,-0.4}, vec3(1,1,1) * 0.5f, quad);

        registry.emplace<Wobbler>(child);

        auto floor = CreateQuadNew(registry, {0,0,0}, {10,10,1});
        auto rot = glm::radians(vec3(90,0,0));
        registry.get<LocalTransform>(floor).rotation = quat(rot);
         */
    }

    void HandleEvent(void* event) override {
        gui.HandleEvent(event);
    }

    void OnGUI() {
        ImGui::DockSpaceOverViewport();

        EditorSimulation* currentSimulation;
        if (editorSimulationRegistry.TryGetFirst(&currentSimulation)) {
            currentSimulation->DrawGUI();
        }

        projectWindow.Draw(project, resourceManager);
    }

    void Update(float dt) override {
        EditorSimulation* currentSimulation;
        if (editorSimulationRegistry.TryGetFirst(&currentSimulation)) {
            currentSimulation->Update(dt);
        }
        simulation.Update(dt);
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
