//
// Created by Jeppe Nielsen on 04/10/2025.
//

#include "EditorSimulation.hpp"

using namespace LittleCore;

EditorSimulation::EditorSimulation(EditorSimulationContext& context, SimulationBase& simulation)
: context(context), simulation(simulation), sceneView(context.netimguiClientController) {
    cameraController.CreateCamera();
}

void EditorSimulation::DrawGUI() {

    hierarchyWindow.Draw(*this);
    sceneView.Draw(*this);
    inspectorWindow.Draw(*this);

}
