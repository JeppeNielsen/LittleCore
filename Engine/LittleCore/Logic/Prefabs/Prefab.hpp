//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceHandle.hpp"
#include "PrefabResource.hpp"
#include "SerializedPrefabComponent.hpp"
#include "RegistryHelper.hpp"
#include <glaze/glaze.hpp>

namespace LittleCore {
    struct Prefab {
        ResourceHandle<PrefabResource> resource;
        std::vector<entt::entity> roots;
        using Components = std::vector<SerializedPrefabComponent>;
        Components components;

        void Cloned(const Prefab& other, const std::unordered_map<entt::entity, entt::entity>& originalToDuplicate) {
            resource = other.resource;
            components = other.components;
            for (int i = 0; i < roots.size(); ++i) {
                roots[i]=RegistryHelper::GetDuplicated(originalToDuplicate, roots[i]);
            }
            for (int i = 0; i < components.size(); ++i) {
                components[i].entity = RegistryHelper::GetDuplicated(originalToDuplicate, components[i].entity);
            }
        }
    };
}

template<>
struct glz::meta<LittleCore::Prefab> {
    using T = LittleCore::Prefab;

    static constexpr auto value = glz::object(
        "resource", &T::resource,
        "components", &T::components
    );
};

