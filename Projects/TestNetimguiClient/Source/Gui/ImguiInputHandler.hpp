//
// Created by Jeppe Nielsen on 08/10/2025.
//


#pragma once
#include "InputHandler.hpp"

namespace LittleCore {
    class ImguiInputHandler : public InputHandler {
        public:
        ImguiInputHandler();
        ~ImguiInputHandler();

        bool isMouseInWindow;
        float mouseX;
        float mouseY;
        void HandleInput(void* event, Input& input) override;
    };
}
