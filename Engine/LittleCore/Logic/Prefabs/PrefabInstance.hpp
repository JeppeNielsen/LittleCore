//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceHandle.hpp"
#include "PrefabResource.hpp"
#include "glaze/glaze.hpp"

namespace LittleCore {
    struct PrefabInstance {
        ResourceHandle<PrefabResource> Prefab;
        std::vector<entt::entity> roots;

        PrefabInstance() = default;

        PrefabInstance(const PrefabInstance& other) {
            Prefab = other.Prefab;
            roots.clear();
        }
    };
}

