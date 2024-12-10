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
    renderOctreeSystem(registry),
    renderSystem(registry, renderOctreeSystem),
    inputSystem(registry)
{

}

void SimpleSimulation::Update() {
    inputSystem.Update();
    hierarchySystem.Update();
    meshBoundingBoxSystem.Update();
    worldBoundingBoxSystem.Update();
    worldTransformSystem.Update();
    renderOctreeSystem.Update();
}

void SimpleSimulation::Render(BGFXRenderer& bgfxRenderer) {
    renderSystem.Render(&bgfxRenderer);
}

InputSystem& SimpleSimulation::Input() {
    return inputSystem;
}