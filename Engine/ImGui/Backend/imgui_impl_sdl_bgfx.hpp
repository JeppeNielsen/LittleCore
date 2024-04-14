#pragma once
#include <bgfx/bgfx.h>
#include "imgui.h"
#include <functional>

struct SDL_Window;

//namespace LittleCore {

    void ImGui_Impl_sdl_bgfx_Init(int view);
    void ImGui_Impl_sdl_bgfx_Shutdown();
    void ImGui_Impl_sdl_bgfx_NewFrame();

    void ImGui_Impl_sdl_bgfx_Resize(SDL_Window *);
    void ImGui_Impl_sdl_bgfx_Render(const bgfx::ViewId viewId, ImDrawData *draw_data, uint32_t clearColor);
    void ImGui_Impl_sdl_bgfx_InvalidateDeviceObjects();
    bool ImGui_Impl_sdl_bgfx_CreateDeviceObjects();
    void SetRenderFunction(std::function<void()> renderFunction);
    unsigned long native_window_handle(ImGuiViewport *viewport, SDL_Window *window);

//}