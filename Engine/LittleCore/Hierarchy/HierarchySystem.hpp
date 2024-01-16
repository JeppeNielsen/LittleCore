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
        void Update() const;

    private:
        entt::registry& registry;
        entt::observer observer;
    };
}
