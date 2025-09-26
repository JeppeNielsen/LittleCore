//
// Created by Jeppe Nielsen on 19/09/2025.
//


#pragma once
#include <entt/entt.hpp>

namespace LittleCore {
    class EntityGuiDrawer {
    public:
        void Draw(entt::registry& registry, entt::entity entity);
    };
}
