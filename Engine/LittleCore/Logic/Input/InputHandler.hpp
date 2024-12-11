//
// Created by Jeppe Nielsen on 10/12/2024.
//

#pragma once
#include "Input.hpp"

namespace LittleCore {
    struct InputHandler {

        virtual ~InputHandler() {};

        virtual void HandleInput(void* event, Input& input) = 0;

    };


}