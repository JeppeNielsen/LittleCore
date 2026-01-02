//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once

#include "GizmoDrawer.hpp"
#include "NetimguiClientController.hpp"
#include "GameWindow.hpp"

namespace LittleCore {

    class EditorSimulation;
    class EditorCamera;

    class SceneWindow {
    public:
        SceneWindow(NetimguiClientController& netimguiClientController, GameWindow& gameWindow);

        void Draw(EditorSimulation& simulation);
        void DrawCamera(EditorSimulation& simulation, EditorCamera& camera);

        GizmoDrawer gizmoDrawer;

    private:
        NetimguiClientController& netimguiClientController;
        GameWindow& gameWindow;
        GuiWindowInputController guiWindowInputController;
    };
}
