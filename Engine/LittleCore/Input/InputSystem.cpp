//
// Created by Jeppe Nielsen on 07/02/2024.
//

#include "InputSystem.hpp"
#include <SDL3/SDL.h>
#include <iostream>
#include "InputKeyMapper.hpp"

using namespace LittleCore;

InputSystem::InputSystem(entt::registry &registry) : registry(registry){
}

void InputSystem::BeginEvents() {
    currentInput = {};
    auto mouseButtonState = SDL_GetMouseState(&currentInput.touchPosition[0].position.x, &currentInput.touchPosition[0].position.y);
    //std::cout << "x : "<< currentInput.touchPosition[0].position.x << " y: "<< currentInput.touchPosition[0].position.y << std::endl;
}

void InputSystem::HandleEvent(void* eventPtr) {

    SDL_Event& event = *reinterpret_cast<SDL_Event*>(eventPtr);

    switch (event.type) {
        case SDL_EVENT_KEY_UP:
            currentInput.keysUp.push_back(InputKeyMapper::FromId(event.key.keysym.scancode));
            break;

        case SDL_EVENT_KEY_DOWN:
            currentInput.keysDown.push_back(InputKeyMapper::FromId(event.key.keysym.scancode));
            break;
    }
}

void InputSystem::EndEvents() {
    const auto& view = registry.view<Input>();

    for(auto& entity : view) {
        registry.patch<Input>(entity) = currentInput;
    }
}