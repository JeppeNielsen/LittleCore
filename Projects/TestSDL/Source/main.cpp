
#include <SDL3/SDL.h>
#include <SDL3/SDL_syswm.h>
#include <iostream>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>


inline bool sdlSetWindow(SDL_Window* _window)
{
    SDL_SysWMinfo wmi;
    if (!SDL_GetWindowWMInfo(_window, &wmi, SDL_COMPILEDVERSION)) {
        return false;
    }

    bgfx::PlatformData pd;
    /*
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_STEAMLINK
    pd.ndt = wmi.info.vivante.display;
    pd.nwh = wmi.info.vivante.window;
#endif // BX_PLATFORM_
     */

    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);

    return true;
}

static int counter = 0;

int Filter(void* userData, SDL_Event* event) {

    //if (event->type != SDL_EVENT_WINDOW_RESIZED) {
        //return 1;
    //}

    int width;
    int height;
    SDL_GetWindowSizeInPixels((SDL_Window*)userData, &width, &height);

    bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);

    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    bgfx::touch(0);
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter++);
    bgfx::frame();

    return 1;
}

int main() {

    SDL_Init(0);

    SDL_Window* window2 = SDL_CreateWindow("bgfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 600, SDL_WINDOW_RESIZABLE);

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
    init.platformData.ndt = NULL;
    init.platformData.nwh = wmi.info.cocoa.window;
    init.platformData.context = NULL;
    init.platformData.backBuffer = NULL;
    init.platformData.backBufferDS = NULL;

    bgfx::init(init);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF /*purple*/, 1.f, 0);



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
