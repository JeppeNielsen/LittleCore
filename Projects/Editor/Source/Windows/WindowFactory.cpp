//
// Created by Jeppe Nielsen on 31/03/2024.
//

#include "WindowFactory.hpp"
#include "SDL3/SDL.h"

IWindow* WindowFactory::Create() {

    auto sdlWindow = SDL_CreateWindow("Window", 256,256,SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);

    IWindow* window = new IWindow();
    window->prt = sdlWindow;

    return window;
}
