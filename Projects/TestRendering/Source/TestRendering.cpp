//
// Created by Jeppe Nielsen on 08/08/2024.
//

#include "TestRendering.hpp"
#include <SDL3/SDL.h>
#include "imgui.h"
#include "Movable.hpp"
#include "DefaultSimulation.hpp"
#include <iostream>

entt::entity CreateQuadNew(entt::registry& registry, glm::vec3 position, entt::entity parent = entt::null) {

    auto quad = registry.create();
    registry.emplace<LocalTransform>(quad).position = position;
    registry.emplace<WorldTransform>(quad);
    registry.emplace<Hierarchy>(quad).parent = parent;
    auto& mesh = registry.emplace<Mesh>(quad);
    mesh.vertices.push_back({{-1,-1,0}, 0x00FF0000 , {0,0}});
    mesh.vertices.push_back({{1,-1,0}, 0xff000000 , {0,1} });
    mesh.vertices.push_back({{1,1,0}, 0xff000000, {1,1}});
    mesh.vertices.push_back({{-1,1,0}, 0xffFF0000,{1,0}});
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

struct Settings {
    int value;
};

/*
struct System {

    System(Settings& settings) : settings(settings) {}
private:
    Settings& settings;
};

template <typename ...System>
using SystemsList = std::tuple<System...>;

template<typename... Systems>
class World {
public:
    World(Settings& settings)
            : settings(settings),
              systems(createSystems(std::make_index_sequence<sizeof...(Systems)>{}, settings)) {}

private:
    Settings& settings;
    SystemsList<Systems...> systems;

    // Helper function to construct the tuple of systems
    template <std::size_t... Is>
    static SystemsList<Systems...> createSystems(std::index_sequence<Is...>, Settings& settings) {
        return SystemsList<Systems...>{ Systems(settings)... };
    }
};
 */

class Person {
public:
    Person(entt::registry& re) : re(re) {}

    void print() const {

    }

private:
    entt::registry& re;
};

    class HierarchySystemTest {
    public:
        HierarchySystemTest(entt::registry& registry) : registry(registry) {}

        HierarchySystemTest(HierarchySystemTest&& other) : registry(other.registry) {}

    private:
        entt::registry& registry;
        entt::observer observer;
    };


struct TestSystem1 {

    TestSystem1(int& number) : number(number) {}
    TestSystem1(const TestSystem1&) = delete;

    int& number;

};

struct TestSystem2 {

    TestSystem2(int& number) : number(number) {}
    TestSystem2(const TestSystem1&) = delete;

    int& number;

};

template<typename ...T>
struct SystemList {
    using Systems = std::tuple<T...>;

    SystemList(int& number) : systems(std::make_tuple(T(std::forward<int&>(number))...)) {

    }

    Systems systems;
};

template<typename List>
struct TestSimulation {

    TestSimulation(int& number) : systems(List(std::forward<int&>(number))) {}

    List systems;
};


#include <tuple>
#include <iostream>

struct TestSystem1New {
    TestSystem1New(int& number) : number(number) {}

    int& number;

    // Delete copy constructor
    TestSystem1New(const TestSystem1New&) = delete;
    TestSystem1New& operator=(const TestSystem1New&) = delete;

    void Update() {
        int i = number;
        std::cout << "Update from TestSystem1New number = " << std::to_string(i) << "\n";
    }
};

struct TestSystem2New {
    TestSystem2New(int& number) : number(number) {}

    int& number;

    // Delete copy constructor
    //TestSystem2New(const TestSystem2New&) = delete;
    //TestSystem2New& operator=(const TestSystem2New&) = delete;

    void Update() {
        int i = number;
        std::cout << "Update from TestSystem2New number = " << std::to_string(i) << "\n";
    }
};

template<typename ...T>
struct SimulationList {

    SimulationList(int& number) : items(std::tuple<T...>(Get<T>(number)...)) {}

    template<typename O>
    constexpr int& Get(int& n) {
        return std::forward<int&>(n);
    }

    std::tuple<T...> items;
};

template<typename UpdateSystems>
struct TestSimulationNew {

    TestSimulationNew(int& number) : systems(number) {}

    void Update() {
        TupleHelper::for_each(systems.items, [] (auto& updateSystem) {
            updateSystem.Update();
        });
    }

    UpdateSystems systems;
};

struct TestSimulationNew2 {
    TestSimulationNew2(int& number) : systems(std::tuple<TestSystem1New, TestSystem2New>(number, number)) {}

    void Update() {
        TupleHelper::for_each(systems, [] (auto& updateSystem) {
            updateSystem.Update();
        });
    }

    std::tuple<TestSystem1New, TestSystem2New> systems;
};


void TestRendering::Initialize() {

    //using UpdateSystems = UpdateSystems<HierarchySystem, WorldBoundingBoxSystem>;
    //using RenderSystems = RenderSystems<HierarchySystem>;

    //Simulation<UpdateSystems, RenderSystems> sim(registry);

    int testNumber = 45;

    //using TestSimulationSystems = SystemList<TestSystem1, TestSystem2>;

    //TestSimulation<TestSimulationSystems> gg(testNumber);

    using UpdateSystems = SimulationList<TestSystem1New, TestSystem2New>;

    TestSimulationNew<UpdateSystems> sim(testNumber);
    //TestSimulationNew2 sim(testNumber);

    sim.Update();

    testNumber = 32;

    sim.Update();

    return;

    //TestSimulationVar<TestSystem> bla(43);

    //Settings settings;

    //World<System> world(settings);

    //DefaultSimulation defaultSimulation(registry);



    //auto ret = Updates::Create( registry);
    //Updates ::printType<decltype(ret)>();

    //auto ret = Updates::CreateTuple<HierarchySystem>(registry);
    //auto ret2 = Updates::CreateTuple<HierarchySystem>(registry);

    //Simulation<Updates, Renders> sim(registry);

    //std::tuple<HierarchySystem> t = std::make_tuple( std::ref(registry));

    //defaultSimulation.Update();
    //std::tuple<entt::registry&> tupleRegistry = std::make_tuple<entt::registry&>(registry);
    //std::tuple<HierarchySystem> tuple = std::make_tuple(registry);


    /*

     HierarchySystemTest d(registry);

    Person person(registry);

    auto ret = std::make_tuple(person);
    std::tuple<HierarchySystemTest> ret2 = std::make_tuple(HierarchySystemTest(registry));

    auto final = std::tuple_cat(  TupleHelper::as_ref(ret), TupleHelper::as_ref(ret2));

    Updates::printType<decltype(final)>();

     */



    imGuiController.Initialize(mainWindow, [this]() {
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Game");
        ImTextureID textureId = (ImTextureID)(uintptr_t)renderTexture.idx;
        ImGui::Image(textureId, ImVec2((float)renderTextureWidth, (float)renderTextureHeight));

        ImGui::End();

        ImGui::Begin("Scene 2");
        ImTextureID textureId2 = (ImTextureID)(uintptr_t)renderTexture.idx;

        for (int i = 0; i < 10; ++i) {
            ImGui::Image(textureId2, ImVec2((float)256, (float)256));
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
    texture = resources.Create<TextureResource>("B62D424BF40F46359248CDE498930422");

    colorTexture  = bgfx::createUniform("colorTexture",  bgfx::UniformType::Sampler);

    quad1 = CreateQuadNew(registry, {0, 0, 0});
    registry.get<Renderable>(quad1).shader = shader;

    quad2 = CreateQuadNew(registry, {3, 0, 0}, quad1);
    registry.get<Renderable>(quad2).shader = shader;

    auto quad3 = CreateQuadNew(registry, {0, 2, 0}, quad2);
    registry.get<Renderable>(quad3).shader = shader;


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
    registry.patch<LocalTransform>(quad2).position = {0, -1 + sinf(time*0.5f) *1, 0};
    //registry.patch<LocalTransform>(quad1).position = {2 + sinf(time)*2,0,0};

    time += dt;

    //registry.patch<LocalTransform>(cameraEntity).rotation = glm::quat({0, 0, time});
    //registry.patch<Camera>(cameraEntity).fieldOfView = 40;


    simulation.Update();
    movableSystem.Step(registry);
}

void TestRendering::Render() {

    bgfx::setViewFrameBuffer(0, framebuffer);
    //bgfx::setViewRect(0, 0, 0, renderTextureWidth, renderTextureHeight);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    bgfx::touch(0);

    bgfx::setTexture(0, colorTexture, texture->handle);
    simulation.Render(bgfxRenderer);
    bgfx::frame();

    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);

    imGuiController.Render();
}

TestRendering::TestRendering() : simulation(registry), resources(resourcePathMapper) {

}

void TestRendering::HandleEvent(void *event) {
    imGuiController.HandleEvent(event);
    simulation.Input().HandleEvent(event);
}
