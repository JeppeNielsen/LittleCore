//
// Created by Jeppe Nielsen on 11/12/2024.
//

#pragma once
#include "InputHandler.hpp"

namespace LittleCore {

    struct SDLInputHandler : public InputHandler {

        ~SDLInputHandler();

        void HandleInput(void* event, Input& input) override;

    };

}