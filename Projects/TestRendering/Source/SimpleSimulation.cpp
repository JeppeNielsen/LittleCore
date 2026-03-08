//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "SimpleSimulation.hpp"

using namespace LittleCore;

SimpleSimulation::SimpleSimulation(entt::registry &registry) :
    registry(registry),
    hierarchySystem(registry),
    meshBoundingBoxSystem(registry),
    worldBoundingBoxSystem(registry),
    worldTransformSystem(registry),
    renderSystem(registry),
    inputSystem(registry)
{

}

void SimpleSimulation::Update() {
    inputSystem.Update();
    hierarchySystem.Update();
    meshBoundingBoxSystem.Update();
    worldBoundingBoxSystem.Update();
    worldTransformSystem.Update();
    renderSystem.Update();
}

void SimpleSimulation::Render(SokolRenderer& renderer) {
    renderSystem.Render(&renderer);
}

InputSystem& SimpleSimulation::Input() {
    return inputSystem;
}
