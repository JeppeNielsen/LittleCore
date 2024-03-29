//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "Simulation.hpp"

using namespace LittleCore;

Simulation::Simulation(entt::registry &registry) :
    registry(registry),
    hierarchySystem(registry),
    meshBoundingBoxSystem(registry),
    worldBoundingBoxSystem(registry),
    worldTransformSystem(registry),
    renderOctreeSystem(registry),
    renderSystem(registry, renderOctreeSystem),
    inputSystem(registry)
{

}

void Simulation::Update() {
    hierarchySystem.Update();
    meshBoundingBoxSystem.Update();
    worldBoundingBoxSystem.Update();
    worldTransformSystem.Update();
    renderOctreeSystem.Update();
}

void Simulation::Render(BGFXRenderer& bgfxRenderer) {
    renderSystem.Render(&bgfxRenderer);
}

InputSystem &Simulation::Input() {
    return inputSystem;
}
