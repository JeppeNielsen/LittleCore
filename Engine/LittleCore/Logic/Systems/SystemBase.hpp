//
// Created by Jeppe Nielsen on 27/09/2025.
//

#pragma once
#include <entt/entt.hpp>

namespace LittleCore {
    struct SystemBase {
        entt::registry& registry;
        SystemBase(entt::registry& registry);
    };
}
