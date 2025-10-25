//
// Created by Jeppe Nielsen on 25/10/2025.
//

#pragma once
#include <entt/entt.hpp>

namespace LittleCore {
    struct PrefabResource {
        entt::entity root = entt::null;
        entt::registry* registry = nullptr;
    };
}
