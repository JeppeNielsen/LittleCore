//
// Created by Jeppe Nielsen on 16/01/2024.
//

#pragma once
#include <entt/entt.hpp>
#include "Hierarchy.hpp"

namespace LittleCore {
    class HierarchySystem {
    public:
        HierarchySystem(entt::registry& registry);
        void Update();

    private:
        void DestroyRecursive(entt::registry& registry, entt::entity entity);
        void EntityDestroyed(entt::registry& reg, entt::entity entity);
        bool isDestroying;
        entt::registry& registry;
        entt::observer observer;
    };
}
