//
// Created by Jeppe Nielsen on 11/12/2024.
//

#include "InputHandler.hpp"
#include "SDLInputHandler.hpp"
#include "InputKeyMapper.hpp"
#include <sokol_app.h>

using namespace LittleCore;

void SDLInputHandler::HandleInput(void *eventPtr, LittleCore::Input &input) {

    const sapp_event& event = *reinterpret_cast<const sapp_event*>(eventPtr);

    switch (event.type) {
        case SAPP_EVENTTYPE_KEY_UP: {
            auto keyId = InputKeyMapper::FromSappKeyCode(event.key_code);
            if (keyId == InputKey::UNKNOWN) {
                break;
            }
            if (handleKeys || input.IsKeyDown(keyId)) {
                input.keysUp.push_back(keyId);
            }
            break;
        }
        case SAPP_EVENTTYPE_KEY_DOWN:
            if (handleKeys) {
                auto keyId = InputKeyMapper::FromSappKeyCode(event.key_code);
                if (keyId != InputKey::UNKNOWN) {
                    input.keysDown.push_back(keyId);
                }
            }
            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            input.touchPosition[0].position = vec2(event.mouse_x, event.mouse_y);
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (handleDownEvents) {
                input.touchesDown.push_back({0});
            }
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            input.touchesUp.push_back({0});
            break;
        case SAPP_EVENTTYPE_TOUCHES_BEGAN:
        case SAPP_EVENTTYPE_TOUCHES_MOVED:
        case SAPP_EVENTTYPE_TOUCHES_ENDED: {
            if (event.num_touches > 0) {
                input.touchPosition[0].position = vec2(event.touches[0].pos_x, event.touches[0].pos_y);
            }
            if (event.type == SAPP_EVENTTYPE_TOUCHES_BEGAN && handleDownEvents) {
                input.touchesDown.push_back({0});
            }
            if (event.type == SAPP_EVENTTYPE_TOUCHES_ENDED) {
                input.touchesUp.push_back({0});
            }
            break;
        }
    }
}

SDLInputHandler::~SDLInputHandler() {

}
