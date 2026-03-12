//
// Created by Jeppe Nielsen on 14/04/2024.
//

#include "ImGuiController.hpp"
#include "SokolDirect.hpp"
#include <sokol_app.h>
#include <sokol_gfx.h>
#include <util/sokol_imgui.h>

using namespace LittleCore;

ImGuiController::~ImGuiController() {
    Destroy();
}

void ImGuiController::Initialize(void* mainWindow, const ImGuiController::RenderFunction& renderFunction) {
    if (isInitialized) {
        Destroy();
    }

    (void)mainWindow;
    this->renderFunction = renderFunction;

    simgui_setup({
        .color_format = static_cast<sg_pixel_format>(sapp_color_format()),
        .depth_format = static_cast<sg_pixel_format>(sapp_depth_format()),
        .sample_count = sapp_sample_count(),
        .ini_filename = "imgui.ini"
    });

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsLight();

    isInitialized = true;
}

void ImGuiController::HandleEvent(void *event) {
    const auto* sappEvent = static_cast<const sapp_event*>(event);
    if (sappEvent == nullptr || !isInitialized) {
        return;
    }
    simgui_handle_event(sappEvent);
}

void ImGuiController::Render() {
    if (!isInitialized) {
        return;
    }

    const int width = sapp_width();
    const int height = sapp_height();
    if (width <= 0 || height <= 0) {
        return;
    }

    const float dpiScale = sapp_dpi_scale();
    const double deltaTime = sapp_frame_duration();
    simgui_new_frame({
        .width = width,
        .height = height,
        .delta_time = deltaTime > 0.0 ? deltaTime : (1.0 / 60.0),
        .dpi_scale = dpiScale > 0.0f ? dpiScale : 1.0f
    });
    renderFunction();
    if (lc_sg_begin_window_pass()) {
        simgui_render();
        lc_sg_end_pass();
    } else {
        ImGui::Render();
    }
}

void ImGuiController::Destroy() {
    if (!isInitialized) {
        return;
    }

    simgui_shutdown();
    isInitialized = false;
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
    (void)viewId;
    (void)draw_data;
    if (!isInitialized) {
        return;
    }
    simgui_render();
}
