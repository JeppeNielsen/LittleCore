//
// Created by Jeppe Nielsen on 16/01/2024.
//

#pragma once
#include <entt/entt.hpp>
#include <vector>

namespace LittleCore {
    struct Hierarchy {
        entt::entity parent = entt::null;
        entt::entity previousParent = entt::null;

        using Children = std::vector<entt::entity>;
        Children children;
    };
}
