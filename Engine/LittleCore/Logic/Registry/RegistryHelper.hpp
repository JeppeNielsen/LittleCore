//
// Created by Jeppe Nielsen on 24/10/2025.
//


#pragma once
#include <functional>
#include "entt/entt.hpp"

namespace LittleCore {
    class RegistryHelper {
    public:
        using Callback = std::function<void(entt::entity source, entt::entity dest)>;
        static entt::entity Duplicate(entt::registry& registry, entt::entity source, entt::registry& destRegistry, const Callback& callback = nullptr);
        static void TraverseHierarchy(entt::registry& registry, entt::entity root, const std::function<void(entt::entity)>& onEntity);
        static entt::entity FindParent(entt::registry& registry, entt::entity source, const std::function<bool(entt::entity)>& predicate);
    };
}
