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

struct RotationSystem : SystemBase {

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

struct WobblerSystem : SystemBase {

    void Update(float dt) {
        auto view = registry.view<LocalTransform, Wobbler>();
        for(auto[entity, transform, wobbler] : view.each()) {
            transform.position = vec3(0,0,0) + wobbler.direction * sin(wobbler.position);
            wobbler.position += wobbler.speed * dt;
        }
    }

};

struct CameraMovementKey {
    InputKey key;
    vec3 movement;
    bool isMoving = false;
};

struct CameraMovement {
    float forwardSpeed = 1.0f;
    float rotationSpeed = 0.0005f;

    std::vector<CameraMovementKey> movementKeys;

    bool isRotating;

    quat startingRotation;
    vec2 touchPosition;
};

struct CameraMovementSystem : SystemBase {

    void Update(float dt) {
        auto view = registry.view<LocalTransform, const WorldTransform, const Input, CameraMovement>();

        for(auto[entity, transform, worldTransform, input, movement] : view.each()) {

            for(auto& key : movement.movementKeys) {
                if (!key.isMoving && input.IsKeyDown(key.key)) {
                    key.isMoving = true;
                } else if (key.isMoving && input.IsKeyUp(key.key)) {
                    key.isMoving = false;
                }

                if (key.isMoving) {
                    vec3 worldForward = worldTransform.world * vec4(key.movement, 0.0f);
                    transform.position += worldForward * dt * movement.forwardSpeed;
                    registry.patch<LocalTransform>(entity);
                }
            }

            if (!movement.isRotating && input.IsTouchDown({0})) {
                movement.isRotating = true;
                movement.startingRotation = transform.rotation;
                movement.touchPosition = input.touchPosition[0].position;
            } else if (movement.isRotating && input.IsTouchUp({0})) {
                movement.isRotating = false;
            }

            if (movement.isRotating) {

                vec2 delta = movement.touchPosition - input.touchPosition[0].position;

                auto pitch = glm::pitch(movement.startingRotation);
                auto yaw = glm::yaw(movement.startingRotation);
                yaw -= delta.x * movement.rotationSpeed;
                pitch -= delta.y * movement.rotationSpeed;

                const float piHalf = glm::pi<float>() * 0.5f;

                if (pitch<-piHalf) {
                    pitch = -piHalf;
                } else if (pitch>piHalf) {
                    pitch = piHalf;
                }

                transform.rotation = glm::quat({pitch, yaw, 0});
                registry.patch<LocalTransform>(entity);
            }
        }
    }

};



struct TestNetimguiClient : IState {
    ImGuiController gui;
    NetimguiClientController netimguiClientController;

    entt::registry registry;
    CustomSimulation<RotationSystem, WobblerSystem, CameraMovementSystem> simulation;
    SDLInputHandler sdlInputHandler;
    BGFXRenderer renderer;
    ResourcePathMapper resourcePathMapper;
    DefaultResourceManager resourceManager;
    ResizableFrameBuffer frameBuffer;
    ImVec2 gameSize;
    EntityGuiDrawer drawer;

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

            auto& movement = registry.emplace<CameraMovement>(cameraObject);
            movement.movementKeys.push_back({
                InputKey::A,
                vec3(-1,0,0)
            });
            movement.movementKeys.push_back({
                                                    InputKey::D,
                                                    vec3(1,0,0)
                                            });
            movement.movementKeys.push_back({
                                                    InputKey::W,
                                                    vec3(0,0,1)
                                            });
            movement.movementKeys.push_back({
                                                    InputKey::S,
                                                    vec3(0,0,-1)
                                            });
            registry.emplace<Input>(cameraObject);
        }

        auto quad = CreateQuadNew(registry, {0, 0, 0}, {1,1,1});
        registry.emplace<Rotatable>(quad);

        auto child = CreateQuadNew(registry, {1,1,-0.4}, vec3(1,1,1) * 0.5f, quad);
        registry.emplace<Rotatable>(child);

        //registry.emplace<Wobbler>(child);
        registry.emplace<Wobbler>(quad);

        auto floor = CreateQuadNew(registry, {0,0,0}, {10,10,1});
        auto rot = glm::radians(vec3(90,0,0));
        registry.get<LocalTransform>(floor).rotation = quat(rot);

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

