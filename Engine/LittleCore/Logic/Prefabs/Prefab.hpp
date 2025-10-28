//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceHandle.hpp"
#include "PrefabResource.hpp"
#include "glaze/glaze.hpp"

namespace LittleCore {
    struct Prefab {
        ResourceHandle<PrefabResource> resource;
        std::vector<entt::entity> roots;

        Prefab() = default;

        Prefab(const Prefab& other) {
            resource = other.resource;
            roots.clear();
        }
    };
}

