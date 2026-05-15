//
// Created by Jeppe Nielsen on 08/10/2025.
//


#pragma once

#include <ResizableFrameBuffer.hpp>
#include <GuiWindowInputController.hpp>

namespace LittleCore {
    class EditorSimulation;

    class GameWindow {
    public:
        GameWindow();

        void Draw(EditorSimulation& simulation);

        float Aspect();

    private:
        ResizableFrameBuffer frameBuffer;
        GuiWindowInputController guiWindowInputController;
    };
}
