//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabSystem.hpp"
#include "RegistryHelper.hpp"
#include "IgnoreSerialization.hpp"

using namespace LittleCore;

PrefabSystem::PrefabSystem(entt::registry& registry) : SystemBase(registry),
    observer(registry, entt::collector
            .update<Prefab>().where<Hierarchy>()
            .group<Prefab, Hierarchy>()) {

}

void PrefabSystem::Update() {
    if (observer.empty()) {
        return;
    }

    while (!observer.empty()) {
        std::vector<entt::entity> changed(observer.begin(), observer.end());
        observer.clear();
        for (auto e : changed) {
            RefreshInstance(e);
        }
    }
}

void PrefabSystem::RefreshInstance(entt::entity entity) {
    Prefab& prefab = registry.get<Prefab>(entity);

    for(auto root : prefab.roots) {
        registry.destroy(root);
    }
    prefab.roots.clear();

    if (!prefab.resource) {
        return;
    }

    auto& resource = *prefab.resource.operator->();

    for(auto rootToDuplicate : resource.roots) {
        auto root = RegistryHelper::Duplicate(*resource.registry, rootToDuplicate, registry);

        RegistryHelper::TraverseHierarchy(registry, root, [&](auto child) {
            registry.emplace<IgnoreSerialization>(child);
        });

        prefab.roots.push_back(root);

        auto& rootHierarchy = registry.get<Hierarchy>(root);
        rootHierarchy.parent = entity;
    }
}
