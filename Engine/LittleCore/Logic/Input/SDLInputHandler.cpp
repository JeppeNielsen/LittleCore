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
        case SDL_EVENT_KEY_UP: {
            auto keyId = InputKeyMapper::FromId(event.key.keysym.scancode);
            if (handleKeys || input.IsKeyDown(keyId)) {
                input.keysUp.push_back(keyId);
            }
            break;
        }
        case SDL_EVENT_KEY_DOWN:
            if (handleKeys) {
                input.keysDown.push_back(InputKeyMapper::FromId(event.key.keysym.scancode));
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            input.touchPosition[0].position = vec2(event.motion.x, event.motion.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (handleDownEvents) {
                input.touchesDown.push_back({0});
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            input.touchesUp.push_back({0});
            break;
    }
}

SDLInputHandler::~SDLInputHandler() {

}
