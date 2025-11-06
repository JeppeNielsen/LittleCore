//
// Created by Jeppe Nielsen on 16/01/2024.
//

#include "HierarchySystem.hpp"
#include <iostream>

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
        auto it = std::find(parentChildren.begin(), parentChildren.end(), entity);

        if (it == parentChildren.end()) {
             auto message = std::format("entity: {}, does not exist in parent child list{}", (int)entity, (int)hierarchy.parent);
             throw std::runtime_error(message);
             //std::cout << "entity: " << (int)entity <<", does not exist in parent child list"<<(int)hierarchy.parent<<"\n";
        } else {
            auto message = std::format("Removing {}, from child list {}", (int)entity, (int)hierarchy.parent);
            std::cout << message << "\n";
            parentChildren.erase(it);
            hierarchy.parent = entt::null;
        }
    }

    for(auto child : hierarchy.children) {
        DestroyRecursive(reg, child);
    }
}


