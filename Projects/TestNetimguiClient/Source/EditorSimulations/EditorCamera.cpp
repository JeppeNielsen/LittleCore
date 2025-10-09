//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "LocalTransform.hpp"
#include "Camera.hpp"
#include "EditorCamera.hpp"

using namespace LittleCore;

EditorCamera::EditorCamera()  {

    auto& registry = simulation.registry;

    cameraEntity = registry.create();

    registry.emplace<Hierarchy>(cameraEntity);

    auto localPosition = registry.emplace<LocalTransform>(cameraEntity);
    localPosition.position = {-5,5,-40};

    WorldTransform& worldTransform = registry.emplace<WorldTransform>(cameraEntity);
    worldTransform.world = localPosition.GetLocalToParent();
    worldTransform.worldInverse = glm::inverse(worldTransform.world);

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

}
