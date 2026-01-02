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
sceneView(context.netimguiClientController, gameWindow),
pickingSystem(simulation.registry){
    cameraController.CreateCamera(pickingSystem);
}

void EditorSimulation::DrawGUI() {

    hierarchyWindow.Draw(*this);
    sceneView.Draw(*this);
    inspectorWindow.Draw(*this);
    gameWindow.Draw(*this);

}

void EditorSimulation::Update(float dt) {
    pickingSystem.Update();
    cameraController.Update(dt);
}

void EditorSimulation::Reload() {
    simulation.Reload();
}