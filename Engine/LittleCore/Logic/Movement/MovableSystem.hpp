//
// Created by Jeppe Nielsen on 16/11/2024.
//


#pragma once
#include <entt/entt.hpp>

namespace LittleCore {
    class MovableSystem {
    public:
        MovableSystem(entt::registry &registry);
        void Update();
    private:
        entt::registry& registry;
    };
}


