//
// Created by Jeppe Nielsen on 13/09/2025.
//


#pragma once
#include "ImGuiController.hpp"

namespace LittleCore {
    class NetimguiController {
    public:
        NetimguiController(ImGuiController& guiController);
        ~NetimguiController();
        bool Start();
        void Update();
        void Draw();

    private:
        ImGuiController& guiController;

    };
}
