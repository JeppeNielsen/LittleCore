//
// Created by Jeppe Nielsen on 07/02/2024.
//

#pragma once
#include <entt/entt.hpp>
#include "Input.hpp"

namespace LittleCore {
    class InputSystem {
    public:
        InputSystem(entt::registry& registry);
        void HandleEvent(void* event);
        void Update();
    private:
        entt::registry& registry;
        Input currentInput;
    };
}