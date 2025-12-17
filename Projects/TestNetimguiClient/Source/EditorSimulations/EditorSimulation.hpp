//
// Created by Jeppe Nielsen on 04/10/2025.
//


#pragma once

#include "Simulation.hpp"
#include "EditorSimulationContext.hpp"
#include "EditorSelection.hpp"
#include "EditorCameraController.hpp"
#include "Renderer.hpp"
#include "HierarchyWindow.hpp"
#include "SceneWindow.hpp"
#include "InspectorWindow.hpp"
#include "NetimguiServerController.hpp"
#include "GameWindow.hpp"
#include "CameraPickerSystem.hpp"

namespace LittleCore {
    struct EditorSimulation {
        EditorSimulation(EditorSimulationContext& context, SimulationBase& simulation);

        void DrawGUI();
        void Update(float dt);
        void Reload();

        EditorSimulationContext& context;
        SimulationBase& simulation;
        entt::registry editorRegistry;
        EditorSelection selection;
        EditorCameraController cameraController;

    private:
        GameWindow gameWindow;
        HierarchyWindow hierarchyWindow;
        SceneWindow sceneView;
        InspectorWindow inspectorWindow;
        PickingSystem<> pickingSystem;
    };
}