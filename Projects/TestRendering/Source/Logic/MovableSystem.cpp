//
// Created by Jeppe Nielsen on 16/11/2024.
//

#include "MovableSystem.hpp"
#include "Input.hpp"
#include "Movable.hpp"
#include "LocalTransform.hpp"

void MovableSystem::Step(entt::registry &registry) {
    for(auto e : registry.view<Input, Movable, LocalTransform>()) {
        auto& input = registry.get<Input>(e);
        auto& movable = registry.get<Movable>(e);

        for(auto& movementKey : movable.keys){

            if (input.IsKeyDown(movementKey.key)) {
                movementKey.isActive = true;
            }

            if (input.IsKeyUp(movementKey.key)) {
                movementKey.isActive = false;
            }

            if (movementKey.isActive) {
                auto& transform = registry.patch<LocalTransform>(e);
                transform.position += movementKey.direction * 0.02f;

            }
        }
    }
}
