//
// Created by Jeppe Nielsen on 12/04/2024.
//

#include "Engine.hpp"
#include <sokol_app.h>
#include "SokolDirect.hpp"

using namespace LittleCore;

Engine::Engine(EngineSettings settings) : settings(std::move(settings)) {

}

void Engine::AppInit(void* userData) {
    static_cast<Engine*>(userData)->OnAppInit();
}

void Engine::AppFrame(void* userData) {
    static_cast<Engine*>(userData)->OnAppFrame();
}

void Engine::AppCleanup(void* userData) {
    static_cast<Engine*>(userData)->OnAppCleanup();
}

void Engine::AppEvent(const sapp_event* event, void* userData) {
    static_cast<Engine*>(userData)->OnAppEvent(event);
}

void Engine::OnAppInit() {
    if (!lc_sg_setup()) {
        sapp_request_quit();
        return;
    }
    graphicsInitialized = true;

    if (onInitializeCallback) {
        onInitializeCallback();
    }

    if (state) {
        state->mainWindow = nullptr;
        state->Initialize();
    }
}

void Engine::OnAppFrame() {
    if (!graphicsInitialized || !state) {
        return;
    }

    const float dt = static_cast<float>(sapp_frame_duration());
    state->Update(dt);
    state->Render();
    lc_sg_commit_frame();
}

void Engine::OnAppCleanup() {
    if (onDestroyCallback) {
        onDestroyCallback();
    }

    if (graphicsInitialized) {
        lc_sg_shutdown();
        graphicsInitialized = false;
    }
}

void Engine::OnAppEvent(const sapp_event* event) {
    if (state) {
        state->HandleEvent((void*)event);
    }
}

void Engine::MainLoop(const std::function<void()>& onInitialize, const std::function<void()>& onDestroy) {
    onInitializeCallback = onInitialize;
    onDestroyCallback = onDestroy;

    sapp_desc desc{};
    desc.init_userdata_cb = &Engine::AppInit;
    desc.frame_userdata_cb = &Engine::AppFrame;
    desc.cleanup_userdata_cb = &Engine::AppCleanup;
    desc.event_userdata_cb = &Engine::AppEvent;
    desc.user_data = this;

    desc.width = 800;
    desc.height = 600;
    desc.high_dpi = true;
    desc.sample_count = 1;
    desc.enable_clipboard = true;
    desc.window_title = settings.mainWindowTitle.c_str();

    sapp_run(desc);
}
