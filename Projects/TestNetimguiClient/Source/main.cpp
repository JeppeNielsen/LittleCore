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
#include "ObjectGuiDrawer.hpp"
#include <glm/glm.hpp>

using namespace LittleCore;

template<>
struct glz::meta<glm::vec<3, float>> {
    //using T = glm::vec<3, float>;
    // reflect as an array [x, y, z]
    /*static constexpr auto value = glz::object(
            "x", &T::x,
            "y", &T::y,
            "z", &T::z
    );
     */

    static constexpr auto value = glz::object(

    );
};

template<>
struct glz::meta<glm::quat> {
    static constexpr auto value = glz::object();
};

template<>
struct glz::meta<glm::vec2> {
    static constexpr auto value = glz::object();
};


struct Rotatable {
    float speed = 0.0f;
    float speedY = 0.0f;
};

struct Player {
    std::string name;
    int age = 12;
    float percentage = 0.5f;
    glm::vec3 position = {0,0,0};
    glm::vec2 size = {1,1};
};

struct RotationSystem  {

    entt::registry& registry;

    RotationSystem(entt::registry& registry) : registry(registry) {}


    void Update() {

        auto view = registry.view<const Rotatable, LocalTransform>();
        for(auto[entity, rotatable, transform] : view.each()) {
            auto roll = glm::roll(transform.rotation);
            auto yaw = glm::yaw(transform.rotation);

            transform.rotation = glm::quat({0, yaw + rotatable.speedY,roll + rotatable.speed});
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
    bgfx::FrameBufferHandle fb;
    bgfx::TextureHandle tex;
    bgfx::TextureHandle texCopy;
    std::vector<uint8_t> pixels;
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

        netimguiClient.Start();
        netimguiClient.Connect("Test client", "localhost");

        while (netimguiClient.IsConnectionPending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds (16));
        }
        if (!netimguiClient.IsConnected()) {
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

        int width;
        int height;
        SDL_GetWindowSizeInPixels((SDL_Window*)mainWindow, &width, &height);
        renderer.screenSize = {width, height};


        const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;
        const uint64_t kTexFlags = BGFX_TEXTURE_RT; // RT texture; sampler filtering is set when binding.

        tex = bgfx::createTexture2D(
                512,
                512,
                false,
                1,
                kFormat,
                kTexFlags
        );

        fb = bgfx::createFrameBuffer(1, &tex, /* destroyTextures = */ true);

        texCopy = bgfx::createTexture2D(
                512, 512, false, 1, bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST
        );

        pixels.resize(512*512*4);


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
        ImGui::Image((void*)(uintptr_t)(tex.idx), {512,512});

        ImGui::End();

        ImGui::Begin("Inspector");

        GuiHelper::DrawHeader("Player");

        ObjectGuiDrawer::Draw(player);

        DrawEntity(quad);
        DrawEntity(child);

        ImGui::End();


    }

    void DrawEntity(entt::entity e) {

        ImGui::PushID((int)e);
        if (registry.all_of<Rotatable>(e)) {
            auto& rotatable = registry.get<Rotatable>(e);

            GuiHelper::DrawHeader("Rotatable");
            ObjectGuiDrawer::Draw(rotatable);

        }

        auto& transform = registry.get<LocalTransform>(e);

        GuiHelper::DrawHeader("Transform");
        ObjectGuiDrawer::Draw(transform);

        ImGui::PopID();
    }

    void Update(float dt) override {
        simulation.Update();

        bgfx::setViewFrameBuffer(0, fb);
        const uint32_t rgba =
                (uint32_t(1 * 255.f) << 24) |
                (uint32_t(1 * 255.f) << 16) |
                (uint32_t(1 * 255.f) <<  8) |
                (uint32_t(1 * 255.f) <<  0);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR, rgba);
        simulation.Render(renderer);
        bgfx::touch(0);
        bgfx::frame();
        bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);

        netimguiClient.SendTexture(tex, texCopy, 512, 512, pixels);
    }

    void Render() override {


        gui.Render();

    }
};

int main() {
    Engine e({"Netimgui Client", false});
    e.Start<TestNetimguiClient>();
    return 0;
}

