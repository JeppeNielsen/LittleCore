//
// Created by Jeppe Nielsen on 16/01/2024.
//

#pragma once
#include <entt/entt.hpp>
#include <vector>
#include "RegistryHelper.hpp"

namespace LittleCore {
    struct Hierarchy {
        entt::entity parent = entt::null;
        entt::entity previousParent = entt::null;

        using Children = std::vector<entt::entity>;
        Children children;

        void Cloned(const Hierarchy& original, const std::unordered_map<entt::entity, entt::entity>& originalToDuplicate) {
            parent = RegistryHelper::GetDuplicated(originalToDuplicate, parent);
            previousParent = entt::null;
            children.clear();
        }

    };
}
