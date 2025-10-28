//
// Created by Jeppe Nielsen on 25/10/2025.
//

#pragma once
#include <entt/entt.hpp>
#include <vector>

namespace LittleCore {
    struct PrefabResource {
        std::vector<entt::entity> roots;
        entt::registry* registry = nullptr;
    };
}
