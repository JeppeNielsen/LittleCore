//
// Created by Jeppe Nielsen on 16/01/2024.
//

#include "HierarchySystem.hpp"
#include <iostream>
#include "RegistryHelper.hpp"

using namespace LittleCore;

HierarchySystem::HierarchySystem(entt::registry& registry) :
    registry(registry),
    observer(registry, entt::collector.update<Hierarchy>().group<Hierarchy>()),
    isDestroying(false) {
    registry.on_destroy<Hierarchy>().connect<&HierarchySystem::EntityDestroyed>(this);
}

void HierarchySystem::Update() {
    for (auto entity : observer) {
        auto& hierarchy = registry.get<Hierarchy>(entity);
        if (hierarchy.previousParent != entt::null) {
            auto& oldParentHierarchy = registry.get<Hierarchy>(hierarchy.previousParent);
            auto& parentChildren = oldParentHierarchy.children;
            parentChildren.erase(std::find(parentChildren.begin(), parentChildren.end(), entity));
        }

        if (hierarchy.parent != entt::null) {
            auto& parentHierarchy = registry.get<Hierarchy>(hierarchy.parent);
            parentHierarchy.children.push_back(entity);
        }

        hierarchy.previousParent = hierarchy.parent;
    }

    observer.clear();
}

void HierarchySystem::EntityDestroyed(entt::registry& r, entt::entity entity) {
    if (isDestroying) {
        return;
    }

    if (!registry.valid(entity)) {
        return;
    }

    auto& hierarchy = registry.get<Hierarchy>(entity);

    if (hierarchy.parent != entt::null && registry.any_of<Hierarchy>(hierarchy.parent)) {
        auto& parentHierarchy = registry.get<Hierarchy>(hierarchy.parent);
        auto& parentChildren = parentHierarchy.children;
        auto it = std::find(parentChildren.begin(), parentChildren.end(), entity);
        parentChildren.erase(it);
        hierarchy.parent = entt::null;
    }

    std::vector<entt::entity> childrenToDestroy;

    RegistryHelper::TraverseHierarchy(registry, entity, [&childrenToDestroy](entt::entity child) {
         childrenToDestroy.push_back(child);
    });

    isDestroying = true;
    for (int i = 1; i < childrenToDestroy.size(); ++i) {
        registry.destroy(childrenToDestroy[i]);
    }
    isDestroying = false;
}


