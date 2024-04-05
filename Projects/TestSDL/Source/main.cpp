
#include <SDL3/SDL.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>


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
    bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter);
    bgfx::frame();

    return 1;
}

int main() {

    SDL_Init(0);

    SDL_Window* window = SDL_CreateWindow("bgfx", 800, 600, SDL_WINDOW_RESIZABLE);

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
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495ED, 1.f, 0);

    SDL_Event event;
    bool exit = false;

    SDL_SetEventFilter(&Filter, window);

    while (!exit) {

        while (SDL_PollEvent(&event)) {

            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                    std::cout << event.key.keysym.scancode << std::endl;
                    break;
                case SDL_EVENT_QUIT:
                    exit = true;
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

        bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);

        bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        bgfx::touch(0);
        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter++);
        bgfx::frame();
    }

    bgfx::shutdown();

    return 0;
}
