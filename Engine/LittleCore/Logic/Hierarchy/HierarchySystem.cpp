//
// Created by Jeppe Nielsen on 16/01/2024.
//

#include "HierarchySystem.hpp"

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

void HierarchySystem::DestroyRecursive(entt::registry& registry, entt::entity entity) {
    if (!registry.any_of<Hierarchy>(entity)) {
        return;
    }

    auto& hierarchy = registry.get<Hierarchy>(entity);

    for(auto child : hierarchy.children) {
        DestroyRecursive(registry, child);
    }

    isDestroying = true;
    registry.destroy(entity);
    isDestroying = false;
}

void HierarchySystem::EntityDestroyed(entt::registry& reg, entt::entity entity) {
    if (isDestroying) {
        return;
    }

    auto& hierarchy = reg.get<Hierarchy>(entity);

    if (hierarchy.parent != entt::null && registry.any_of<Hierarchy>(hierarchy.parent)) {
        auto& parentHierarchy = registry.get<Hierarchy>(hierarchy.parent);
        auto& parentChildren = parentHierarchy.children;
        parentChildren.erase(std::find(parentChildren.begin(), parentChildren.end(), entity));
    }

    for(auto child : hierarchy.children) {
        DestroyRecursive(reg, child);
    }
}


