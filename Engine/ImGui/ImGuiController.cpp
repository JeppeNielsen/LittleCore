//
// Created by Jeppe Nielsen on 14/04/2024.
//

#include "ImGuiController.hpp"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include "Backend/imgui_impl_sdl_bgfx.hpp"

using namespace LittleCore;

ImGuiController::~ImGuiController() {
    Destroy();
}

void ImGuiController::Initialize(void* mainWindow, const ImGuiController::RenderFunction& renderFunction) {
    this->mainWindow = mainWindow;
    this->renderFunction = renderFunction;

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;
    io.WantTextInput = true;

    ImGui::StyleColorsLight();

    ImGuiStyle &style = ImGui::GetStyle();

    /*style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    style.FrameBorderSize = 0.f;
    style.FramePadding = ImVec2(1.f, 1.f);
*/
    ImGui_Impl_sdl_bgfx_Init(0);
    SetRenderFunction([this] () {
        Render();
    });
    ImGui_ImplSDL3_InitForMetal((SDL_Window *) mainWindow);
}

void ImGuiController::HandleEvent(void *event) {
    SDL_Event* sdlEvent = (SDL_Event*)event;
    ImGui_ImplSDL3_ProcessEvent(sdlEvent);
}

void ImGuiController::Render() {

    ImGui_Impl_sdl_bgfx_Resize((SDL_Window*)mainWindow);

    ImGui_ImplSDL3_NewFrame();
    ImGui_Impl_sdl_bgfx_NewFrame();

    ImGui::NewFrame();

    renderFunction();

    ImGui::Render();

    ImGui_Impl_sdl_bgfx_Render(0, ImGui::GetDrawData(), 0);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    bgfx::touch(0);

    bgfx::frame();
}

void ImGuiController::Destroy() {
    ImGui_ImplSDL3_Shutdown();
    ImGui_Impl_sdl_bgfx_Shutdown();
}

ImFont* ImGuiController::LoadFont(const std::string &fontPath, float fontSize) {
    ImGuiIO &io = ImGui::GetIO();

    static ImFontConfig config;
    //config.OversampleH = 3;
    //config.OversampleV = 3;
    config.RasterizerDensity = 2.0f;

    return io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, &config);
}

void ImGuiController::Draw(uint16_t viewId, ImDrawData* draw_data) {
    ImGui_Impl_sdl_bgfx_Render(viewId, draw_data, 0);
}

