//
// Created by Jeppe Nielsen on 24/10/2025.
//


#pragma once
#include "entt/entt.hpp"

namespace LittleCore {
    class RegistryHelper {
    public:
        static entt::entity Duplicate(entt::registry& registry, entt::entity source);

    };
}
