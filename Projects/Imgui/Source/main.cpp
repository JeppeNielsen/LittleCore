
#include <SDL3/SDL.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "imgui.h"
#include "imgui_impl_sdl_bgfx.h"
#include "backends/imgui_impl_sdl3.h"

using namespace blackboard;

inline bool sdlSetWindow(SDL_Window* _window)
{

    bgfx::PlatformData pd;
    pd.ndt = nullptr;

#if BX_PLATFORM_OSX
    pd.nwh = SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
#elif BX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
#endif

    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
    bgfx::setPlatformData(pd);

    return true;
}

static int counter = 0;
static int width = 800;
static int height = 600;
static SDL_Window* window;

int Filter(void* userData, SDL_Event* event) {

    if (event->type != SDL_EVENT_WINDOW_RESIZED) {
        return 1;
    }

    /*SDL_GetWindowSizeInPixels((SDL_Window*)userData, &width, &height);

    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::touch(0);
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter);
    bgfx::frame();
     */


    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[i];
        if (viewport->Flags & ImGuiViewportFlags_IsMinimized)
            continue;
        //bgfx::reset((uint32_t)viewport->Size.x, (uint32_t)viewport->Size.y, BGFX_RESET_VSYNC);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

    }

    ImGui::RenderPlatformWindowsDefault();


    return 1;
}



void dockspace()
{
   const static ImGuiDockNodeFlags dockspace_flags{ImGuiDockNodeFlags_None};


    const static ImGuiWindowFlags window_flags{
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus};


    const ImGuiViewport *viewport{ImGui::GetMainViewport()};
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);



    // Submit the DockSpace
    ImGuiIO &io{ImGui::GetIO()};
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id{ImGui::GetID("main_dockspace")};
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}


void OnGUI() {

    //dockspace();


    ImGui::DockSpaceOverViewport();

    ImGui::Begin("My test window");



    ImGui::End();


    ImGui::Begin("Window 2");



    ImGui::End();


    ImGui::ShowDemoWindow();

}

void Render() {

    //renderer::ImGui_Impl_sdl_bgfx_Resize(window);


    renderer::ImGui_Impl_sdl_bgfx_NewFrame();

    ImGui::NewFrame();

    OnGUI();

    ImGui::Render();
    renderer::ImGui_Impl_sdl_bgfx_Render(255, ImGui::GetDrawData(), 0);

    if (const auto io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    int width;
    int height;
    SDL_GetWindowSizeInPixels(window, &width, &height);

    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);

    bgfx::setViewRect(255, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::touch(255);
    //bgfx::dbgTextClear();
    //bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter);
    bgfx::frame();

}

int main() {

    uint32_t backgroundColor;

    SDL_Init(0);



    window = SDL_CreateWindow("bgfx", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    SDL_StartTextInput();

    bgfx::renderFrame();

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width = 800;
    init.resolution.height = 600;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.ndt = nullptr;
    init.platformData.nwh = SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    init.platformData.context = nullptr;
    init.platformData.backBuffer = nullptr;
    init.platformData.backBufferDS = nullptr;

    bgfx::init(init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(255, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, backgroundColor, 1.f, 0);



    //SDL_SetEventFilter(&Filter, window);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    // Set up input output configs
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;
    io.WantTextInput = true;

    static ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 3;
    config.RasterizerDensity = 2.0f;

    //io.Fonts->AddFontFromFileTTF("/Users/jeppe/Jeppes/LittleCore/Projects/Imgui/Source/Fonts/SF-Pro-Text-Bold.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("/Users/jeppe/Jeppes/LittleCore/Projects/Imgui/Source/Fonts/SF-Pro-Text-Medium.otf", 16.0f);
    io.Fonts->AddFontFromFileTTF("/Users/jeppe/Jeppes/LittleCore/Projects/Imgui/Source/Fonts/LucidaG.ttf", 14.0f, &config);

    ImGui::StyleColorsLight();

    // Setup friendly style for multiviewport
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.FrameBorderSize = 0.f;
        style.FramePadding = ImVec2(0.f, 0.f);
    }




    renderer::ImGui_Impl_sdl_bgfx_Init(255);
    renderer::SerRenderFunction( Render);
    ImGui_ImplSDL3_InitForMetal(window);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");

    bool exit = false;
    while (!exit) {

        ImGui_ImplSDL3_NewFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (ImGui_ImplSDL3_ProcessEvent(&event)) {
                continue;
            }
            const bool is_main_window = event.window.windowID == SDL_GetWindowID(window);

            if (event.type == SDL_EVENT_QUIT)
                exit = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && is_main_window)
                exit = true;
        }



        bgfx::setViewClear(255, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495ED, 1.f, 0);



        Render();
        counter++;
    }

    renderer::ImGui_Impl_sdl_bgfx_Shutdown();
    bgfx::shutdown();

    return 0;
}
