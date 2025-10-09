//
// Created by Jeppe Nielsen on 04/10/2025.
//

#include "EditorSimulation.hpp"

using namespace LittleCore;

EditorSimulation::EditorSimulation(EditorSimulationContext& context, SimulationBase& simulation)
:
context(context),
simulation(simulation),
gameWindow(context.netimguiClientController),
sceneView(context.netimguiClientController, gameWindow) {
    cameraController.CreateCamera();
}

void EditorSimulation::DrawGUI() {

    hierarchyWindow.Draw(*this);
    sceneView.Draw(*this);
    inspectorWindow.Draw(*this);
    gameWindow.Draw(*this);

}

void EditorSimulation::Update(float dt) {
    cameraController.Update(dt);
}
