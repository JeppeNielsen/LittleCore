//
// Created by Jeppe Nielsen on 16/11/2024.
//

#include "MovableSystem.hpp"
#include "Input.hpp"
#include "Movable.hpp"
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"

using namespace LittleCore;

void MovableSystem::Update(float dt) {
    auto view = registry.view<const Input, Movable, LocalTransform, const WorldTransform>();
    for(auto[entity, input, movable, localTransform, worldTransform] : view.each()) {
        bool hasChanged = false;
        for(auto& movementKey : movable.keys){

            if (!movementKey.isActive && input.IsKeyDown(movementKey.key)) {
                movementKey.isActive = true;
            } else if (movementKey.isActive && input.IsKeyUp(movementKey.key)) {
                movementKey.isActive = false;
            }

            if (movementKey.isActive) {
                vec3 worldForward = worldTransform.world * vec4(movementKey.direction, 0.0f);
                localTransform.position += worldForward * movable.speed * dt;
                hasChanged = true;
            }
        }

        if (hasChanged) {
            registry.patch<LocalTransform>(entity);
        }
    }
}