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
#include "SceneView.hpp"
#include "InspectorWindow.hpp"
#include "NetimguiServerController.hpp"

namespace LittleCore {
    struct EditorSimulation {
        EditorSimulation(EditorSimulationContext& context, SimulationBase& simulation);

        void DrawGUI();

        EditorSimulationContext& context;
        SimulationBase& simulation;
        entt::registry editorRegistry;
        EditorSelection selection;
        EditorCameraController cameraController;

    private:
        HierarchyWindow hierarchyWindow;
        SceneView sceneView;
        InspectorWindow inspectorWindow;
    };
}