//
// Created by Jeppe Nielsen on 11/12/2024.
//

#include "InputHandler.hpp"
#include "SDLInputHandler.hpp"
#include "InputKeyMapper.hpp"
#include <SDL3/SDL.h>

using namespace LittleCore;

void SDLInputHandler::HandleInput(void *eventPtr, LittleCore::Input &input) {

    SDL_Event& event = *reinterpret_cast<SDL_Event*>(eventPtr);

    switch (event.type) {
        case SDL_EVENT_KEY_UP:
            input.keysUp.push_back(InputKeyMapper::FromId(event.key.keysym.scancode));
            break;

        case SDL_EVENT_KEY_DOWN:
            input.keysDown.push_back(InputKeyMapper::FromId(event.key.keysym.scancode));
            break;
    }
}

SDLInputHandler::~SDLInputHandler() {

}
