//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "LocalTransform.hpp"
#include "Camera.hpp"
#include "EditorCamera.hpp"
#include "CameraPicker.hpp"

using namespace LittleCore;

EditorCamera::EditorCamera(PickingSystem<>& pickingSystem) : cameraPickerSystem(simulation.registry, pickingSystem) {

    auto& registry = simulation.registry;

    cameraEntity = registry.create();

    registry.emplace<Hierarchy>(cameraEntity);

    auto& localPosition = registry.emplace<LocalTransform>(cameraEntity);
    localPosition.position = {-5,5,-40};

    registry.emplace<WorldTransform>(cameraEntity);

    auto &camera = registry.emplace<Camera>(cameraEntity);
    camera.fieldOfView = 30.0f;
    camera.near = 1;
    camera.far = 500;
    camera.viewRect = {{0,    0},
                       {1.0f, 1.0f}};

    auto& movable = registry.emplace<Movable>(cameraEntity);
    movable.speed = 10.0f;
    movable.keys.push_back({
                                   InputKey::A,
                                   vec3(-1,0,0)
                           });
    movable.keys.push_back({
                                   InputKey::D,
                                   vec3(1,0,0)
                           });
    movable.keys.push_back({
                                   InputKey::W,
                                   vec3(0,0,1)
                           });
    movable.keys.push_back({
                                   InputKey::S,
                                   vec3(0,0,-1)
                           });
    registry.emplace<Input>(cameraEntity);

    auto& inputRotation = registry.emplace<InputRotation>(cameraEntity);
    registry.emplace<CameraPicker>(cameraEntity);

}

void EditorCamera::Update(float dt) {
    simulation.Update(dt);
    cameraPickerSystem.Update();
}
