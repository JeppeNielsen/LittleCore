//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once

#include "GizmoDrawer.hpp"
#include "NetimguiClientController.hpp"

namespace LittleCore {

    class EditorSimulation;
    class EditorCamera;

    class SceneView {
    public:
        SceneView(NetimguiClientController& netimguiClientController);

        void Draw(EditorSimulation& simulation);
        void DrawCamera(EditorSimulation& simulation, EditorCamera& camera);

        GizmoDrawer gizmoDrawer;

    private:
        NetimguiClientController& netimguiClientController;
    };
}
