//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceHandle.hpp"
#include "PrefabResource.hpp"
#include "SerializedPrefabComponent.hpp"

namespace LittleCore {
    struct Prefab {
        ResourceHandle<PrefabResource> resource;
        std::vector<entt::entity> roots;
        using Components = std::vector<SerializedPrefabComponent>;
        Components components;

        void Cloned(const Prefab& other) {
            resource = other.resource;
            roots.clear();
            components = other.components;
        }
    };
}

