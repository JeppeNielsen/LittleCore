//
// Created by Jeppe Nielsen on 12/04/2024.
//

#include "Engine.hpp"
#include "Timer.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

using namespace LittleCore;


void Engine::MainLoop() {

    const int width = 800;
    const int height = 600;

    SDL_Init(0);

    auto window = SDL_CreateWindow("bgfx", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    state->mainWindow = window;

    SDL_StartTextInput();

    bgfx::renderFrame();

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.ndt = nullptr;
    init.platformData.nwh = SDL_GetProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    init.platformData.context = nullptr;
    init.platformData.backBuffer = nullptr;
    init.platformData.backBufferDS = nullptr;

    bgfx::init(init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.f, 0);

    state->Initialize();

    Timer timer;
    timer.Start();

    bool exit = false;
    while (!exit) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            state->HandleEvent(&event);
            const bool is_main_window = event.window.windowID == SDL_GetWindowID(window);

            if (event.type == SDL_EVENT_QUIT)
                exit = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && is_main_window)
                exit = true;
        }

        float dt = timer.Stop();
        timer.Start();

        state->Update(dt);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        state->Render();
        bgfx::frame();
    }

    bgfx::shutdown();

}
