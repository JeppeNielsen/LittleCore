//
// Created by Jeppe Nielsen on 08/10/2025.
//


#pragma once

#include <NetimguiClientController.hpp>
#include <ResizableFrameBuffer.hpp>
#include <GuiWindowInputController.hpp>

namespace LittleCore {
    class EditorSimulation;

    class GameWindow {
    public:
        GameWindow(NetimguiClientController& netimguiClientController);

        void Draw(EditorSimulation& simulation);

        float Aspect();

    private:
        NetimguiClientController& netimguiClientController;
        ResizableFrameBuffer frameBuffer;
        GuiWindowInputController guiWindowInputController;
    };
}
