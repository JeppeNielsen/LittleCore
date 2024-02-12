
#include <SDL3/SDL.h>
#include <SDL3/SDL_syswm.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <fstream>
#include <vector>
#include "RenderSystem.hpp"
#include "Vertex.hpp"
#include "Simulation.hpp"
#include "Input.hpp"

int Filter(void* userData, SDL_Event* event) {

    if (event->type != SDL_EVENT_WINDOW_RESIZED) {
        return 1;
    }

    int width;
    int height;
    SDL_GetWindowSizeInPixels((SDL_Window*)userData, &width, &height);

    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::touch(0);
    bgfx::dbgTextClear();
    bgfx::frame();

    return 1;
}

std::vector<uint8_t> readFile(const char* filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               std::istream_iterator<uint8_t>(file),
               std::istream_iterator<uint8_t>());

    return vec;
}

using namespace LittleCore;

entt::entity CreateQuad(entt::registry& registry, glm::vec3 position, entt::entity parent = entt::null) {

    auto quad = registry.create();
    registry.emplace<LocalTransform>(quad).position = position;
    registry.emplace<WorldTransform>(quad);
    registry.emplace<Hierarchy>(quad).parent = parent;
    auto& mesh = registry.emplace<Mesh>(quad);
    mesh.vertices.push_back({{-1,-1,0}, 0x00FF0000 });
    mesh.vertices.push_back({{1,-1,0}, 0xff000000 });
    mesh.vertices.push_back({{1,1,0}, 0xff000000});
    mesh.vertices.push_back({{-1,1,0}, 0xff000000});
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

struct MovementKey {
    InputKey key;
    vec3 direction;
    bool isActive;
};

struct Movable {
    std::vector<MovementKey> keys;
};



int main() {

    SDL_Init(0);

    SDL_Window* window = SDL_CreateWindow("bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE);

    SDL_SysWMinfo wmi;
    if (!SDL_GetWindowWMInfo(window, &wmi, SDL_COMPILEDVERSION)) {
        return 0;
    }

    bgfx::renderFrame();

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width = 800;
    init.resolution.height = 600;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.ndt = nullptr;
    init.platformData.nwh = wmi.info.cocoa.window;
    init.platformData.context = nullptr;
    init.platformData.backBuffer = nullptr;
    init.platformData.backBufferDS = nullptr;

    bgfx::init(init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495ED, 1.f, 0);

    auto vertexShader = readFile("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/vs_cubes.bin");
    auto fragShader = readFile("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Source/fs_cubes.bin");

    const bgfx::Memory* vsSource = bgfx::makeRef(&vertexShader[0], vertexShader.size());
    bgfx::ShaderHandle vsShader = bgfx::createShader(vsSource);

    const bgfx::Memory* fsSource = bgfx::makeRef(&fragShader[0], fragShader.size());
    bgfx::ShaderHandle fsShader = bgfx::createShader(fsSource);

    auto program = bgfx::createProgram(vsShader, fsShader, true);

    SDL_Event event;
    bool exit = false;

    SDL_SetEventFilter(&Filter, window);

    {
        float time = 0;

        float scale = 1.0f;


        entt::registry registry;

        Simulation simulation(registry);

        BGFXRenderer bgfxRenderer;

        entt::entity cameraEntity;
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


        /*
        {
            auto cameraObject = registry.create();
            registry.emplace<LocalTransform>(cameraObject).position = {0, 0, -10};
            registry.emplace<WorldTransform>(cameraObject);
            registry.emplace<Hierarchy>(cameraObject);

            auto &camera = registry.emplace<Camera>(cameraObject);
            camera.fieldOfView = 60.0f;
            camera.near = 1;
            camera.far = 20;
            camera.viewRect = {{0.5f,0},{1.0f,1.0f}};


        }*/

        auto quad1 = CreateQuad(registry, {0, 0, 0});
        registry.get<Renderable>(quad1).shaderProgram = program;

        auto quad2 = CreateQuad(registry, {3, 0, 0}, quad1);
        registry.get<Renderable>(quad2).shaderProgram = program;

        auto quad3 = CreateQuad(registry, {0, 2, 0}, quad2);
        registry.get<Renderable>(quad3).shaderProgram = program;

        while (!exit) {

            time += (1.0f / 60.0f) * 0.2f;

            simulation.Input().BeginEvents();

            while (SDL_PollEvent(&event)) {

                simulation.Input().HandleEvent(&event);

                switch (event.type) {
                    case SDL_EVENT_KEY_DOWN:
                        std::cout << event.key.keysym.scancode << std::endl;
                        break;
                    case SDL_EVENT_QUIT:
                        exit = true;
                        break;

                    case SDL_EVENT_MOUSE_WHEEL:
                        scale += event.wheel.y * 0.1f;
                        break;

                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                        auto windowId = event.window.windowID;
                        SDL_DestroyWindow(SDL_GetWindowFromID(windowId));
                    }
                }
            }

            simulation.Input().EndEvents();

            int width;
            int height;
            SDL_GetWindowSizeInPixels(window, &width, &height);
            bgfxRenderer.screenSize = {width, height};

            bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);

            registry.patch<LocalTransform>(quad2).position = {0, 4 + sinf(time) * 4, 0};
            //registry.patch<LocalTransform>(quad1).position = {6 + sinf(time)*6,0,0};
            //registry.patch<LocalTransform>(quad1).rotation =  glm::quat({0,0,scale});

            //registry.patch<LocalTransform>(cameraEntity).rotation = glm::quat({0, 0, time});
            registry.patch<Camera>(cameraEntity).fieldOfView = 10 + scale;


            simulation.Update();

            for(auto e : registry.view<Input, Movable, LocalTransform>()) {
                auto& input = registry.get<Input>(e);
                auto& movable = registry.get<Movable>(e);

                for(auto& movementKey : movable.keys){

                    if (input.IsKeyDown(movementKey.key)) {
                        movementKey.isActive = true;
                    }

                    if (input.IsKeyUp(movementKey.key)) {
                        movementKey.isActive = false;
                    }

                    if (movementKey.isActive) {
                        auto& transform = registry.patch<LocalTransform>(e);
                        transform.position += movementKey.direction * 0.02f;
                    }

                }

            }

            simulation.Render(bgfxRenderer);

            std::cout << "Num meshes :"<<bgfxRenderer.numMeshes<<", num batches:"<< bgfxRenderer.numBatches << "\n";

            bgfx::frame();
        }

    }

    bgfx::destroy(program);

    bgfx::shutdown();

    return 0;
}
