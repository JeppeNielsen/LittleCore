//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabSystem.hpp"
#include "RegistryHelper.hpp"
#include "IgnoreSerialization.hpp"

using namespace LittleCore;

PrefabSystem::PrefabSystem(entt::registry& registry) : SystemBase(registry),
    observer(registry, entt::collector
            .update<PrefabInstance>().where<Hierarchy>()
            .group<PrefabInstance, Hierarchy>()) {

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
    PrefabInstance& prefabInstance = registry.get<PrefabInstance>(entity);

    for(auto root : prefabInstance.roots) {
        registry.destroy(root);
    }
    prefabInstance.roots.clear();

    if (!prefabInstance.Prefab) {
        return;
    }

    auto& resource = *prefabInstance.Prefab.operator->();

    for(auto rootToDuplicate : resource.roots) {
        auto root = RegistryHelper::Duplicate(*resource.registry, rootToDuplicate, registry);

        RegistryHelper::TraverseHierarchy(registry, root, [&](auto child) {
            registry.emplace<IgnoreSerialization>(child);
        });

        prefabInstance.roots.push_back(root);

        auto& rootHierarchy = registry.get<Hierarchy>(root);
        rootHierarchy.parent = entity;
    }
}
