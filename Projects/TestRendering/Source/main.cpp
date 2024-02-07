
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

struct PosColorVertex
{
    /*float m_x;
    float m_y;
    float m_z;
     */
    glm::vec3 pos;

    uint32_t m_abgr;

    static void init() {
        ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
        {
                {{-1.0f,  1.0f,  1.0f}, 0xff000000 },
                {{ 1.0f,  1.0f,  1.0f}, 0xff0000ff },
                {{-1.0f, -1.0f,  1.0f}, 0xff00ff00 },
                {{1.0f, -1.0f,  1.0f}, 0xff00ffff },
                {{-1.0f,  1.0f, -1.0f}, 0xffff0000 },
                { {1.0f,  1.0f, -1.0f}, 0xffff00ff },
                {{-1.0f, -1.0f, -1.0f}, 0xffffff00 },
                { {1.0f, -1.0f, -1.0f}, 0xffffffff },
        };

static const uint16_t s_cubeTriList[] =
        {
                0, 1, 2, // 0
                1, 3, 2,
                4, 6, 5, // 2
                5, 6, 7,
                0, 2, 4, // 4
                4, 2, 6,
                1, 5, 3, // 6
                5, 7, 3,
                0, 4, 1, // 8
                4, 5, 1,
                2, 3, 6, // 10
                6, 3, 7,
        };




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

struct Mat {
    float v[16];
    
    
    operator float*() { return v; }
};

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



    PosColorVertex::init();

    // Create static vertex buffer.
    auto vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
            , PosColorVertex::ms_layout
    );

    // Create static index buffer for triangle list rendering.
    auto ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList) )
    );


    auto vertexShader = readFile("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Metal/vs_cubes.bin");
    auto fragShader = readFile("/Users/jeppe/Jeppes/LittleCore/Projects/Cubes/Shaders/Metal/fs_cubes.bin");


    //std::ifstream infile("vs_cubes", std::ios_base::binary);
    //std::vector<uint8_t> buffer( std::istreambuf_iterator<uint8_t>(infile),
    //                         std::istreambuf_iterator<uint8_t>() );

    const bgfx::Memory* vsSource = bgfx::makeRef(&vertexShader[0], vertexShader.size());
    bgfx::ShaderHandle vsShader = bgfx::createShader(vsSource);

    const bgfx::Memory* fsSource = bgfx::makeRef(&fragShader[0], fragShader.size());
    bgfx::ShaderHandle fsShader = bgfx::createShader(fsSource);

    auto program = bgfx::createProgram(vsShader, fsShader, true);

    auto program2 = bgfx::createProgram(vsShader, fsShader, true);

    //auto m_program = loadProgram("vs_cubes", "fs_cubes");

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

            while (SDL_PollEvent(&event)) {

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

            int width;
            int height;
            SDL_GetWindowSizeInPixels(window, &width, &height);
            bgfxRenderer.screenSize = {width, height};

            bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);

            registry.patch<LocalTransform>(quad2).position = {0, 4 + sinf(time) * 4, 0};
            //registry.patch<LocalTransform>(quad1).position = {6 + sinf(time)*6,0,0};
            //registry.patch<LocalTransform>(quad1).rotation =  glm::quat({0,0,scale});

            registry.patch<LocalTransform>(cameraEntity).rotation = glm::quat({0, 0, time});
            registry.patch<Camera>(cameraEntity).fieldOfView = 10 + scale;


            simulation.Update();
            simulation.Render(bgfxRenderer);

            std::cout << "Num meshes :"<<bgfxRenderer.numMeshes<<", num batches:"<< bgfxRenderer.numBatches << "\n";

            bgfx::frame();
        }

    }

    bgfx::destroy(program);

    bgfx::shutdown();

    return 0;
}