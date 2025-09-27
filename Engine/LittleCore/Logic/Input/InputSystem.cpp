//
// Created by Jeppe Nielsen on 07/02/2024.
//

#include "InputSystem.hpp"

#include <iostream>
#include "InputKeyMapper.hpp"

using namespace LittleCore;

InputSystem::InputSystem(entt::registry &registry) : registry(registry){
}

void InputSystem::HandleEvent(void* eventPtr, InputHandler& inputHandler) {
    inputHandler.HandleInput(eventPtr, currentInput);
}

void InputSystem::Update() {
    const auto& view = registry.view<Input>();
    for(auto& entity : view) {
        registry.patch<Input>(entity) = currentInput;
    }
    currentInput.Clear();
}