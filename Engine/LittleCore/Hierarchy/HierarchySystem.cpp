//
// Created by Jeppe Nielsen on 16/01/2024.
//

#include "HierarchySystem.hpp"

using namespace LittleCore;

HierarchySystem::HierarchySystem(entt::registry& registry) :
    registry(registry),
    observer{registry, entt::collector.update<Hierarchy>()} {
}

void HierarchySystem::Update() const {
    for (auto entity : observer) {
        const auto& hierarchy = registry.get<Hierarchy>(entity);

        if (hierarchy.oldParent != entt::null) {
            auto& oldParentHierarchy = registry.get<Hierarchy>(hierarchy.oldParent);
            auto& parentChildren = oldParentHierarchy.children;
            parentChildren.erase(std::find(parentChildren.begin(), parentChildren.end(), entity));
        }

        if (hierarchy.parent != entt::null) {
            auto& parentHierarchy = registry.get<Hierarchy>(hierarchy.parent);
            parentHierarchy.children.push_back(entity);
        }
    }
}


