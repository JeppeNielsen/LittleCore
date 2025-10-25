//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabSystem.hpp"
#include "RegistryHelper.hpp"

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

    for(auto e : observer) {
        RefreshInstance(e);
    }
    observer.clear();
}

void PrefabSystem::RefreshInstance(entt::entity entity) {
    PrefabInstance& prefabInstance = registry.get<PrefabInstance>(entity);

    if (prefabInstance.root != entt::null) {
        registry.destroy(prefabInstance.root);
        prefabInstance.root = entt::null;
    }

    if (!prefabInstance.Prefab) {
        return;
    }

    auto& resource = *prefabInstance.Prefab.operator->();

    auto root = RegistryHelper::Duplicate(*resource.registry, resource.root, registry);

    prefabInstance.root = root;
    auto& rootHierarchy = registry.get<Hierarchy>(root);
    rootHierarchy.parent = entity;

}
