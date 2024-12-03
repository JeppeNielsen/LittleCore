//
// Created by Jeppe Nielsen on 18/01/2024.
//

#pragma once
#include <entt/entt.hpp>
#include "LocalTransform.hpp"
#include "WorldTransform.hpp"

namespace LittleCore {
    class WorldTransformSystem {
    public:
        WorldTransformSystem(entt::registry& registry);
        void Update();

    private:
        entt::registry& registry;
        entt::observer observer;
    };

} // LittleCore