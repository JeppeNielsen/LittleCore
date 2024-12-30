//
// Created by Jeppe Nielsen on 17/01/2024.
//

#include "gtest/gtest.h"
#include "HierarchySystem.hpp"

using namespace LittleCore;

namespace {

    TEST(HierachySystem, SetParentShouldAppearInChildrenList) {
        entt::registry registry;
        HierarchySystem hierarchySystem(registry);

        auto parent = registry.create();
        auto child = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        auto& childHierarchy = registry.emplace<Hierarchy>(child);

        childHierarchy.parent = parent;
        registry.patch<Hierarchy>(child);

        hierarchySystem.Update();

        auto& parentChildren = registry.get<Hierarchy>(parent).children;
        EXPECT_TRUE(std::find(parentChildren.begin(), parentChildren.end(), child)!=parentChildren.end());
    }

    TEST(HierachySystem, SetParentToNoneShouldBeRemovedInChildrenList) {
        entt::registry registry;
        HierarchySystem hierarchySystem(registry);

        auto parent = registry.create();
        auto child = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        auto& childHierarchy = registry.emplace<Hierarchy>(child);

        childHierarchy.parent = parent;
        registry.patch<Hierarchy>(child);

        hierarchySystem.Update();
        auto& parentChildren = registry.get<Hierarchy>(parent).children;
        bool didExistsInParentChildren = std::find(parentChildren.begin(), parentChildren.end(), child)!=parentChildren.end();

        childHierarchy.parent = entt::null;
        registry.patch<Hierarchy>(child);

        hierarchySystem.Update();

        bool doesNotExistsInParentChildren = std::find(parentChildren.begin(), parentChildren.end(), child)==parentChildren.end();
        EXPECT_TRUE(didExistsInParentChildren && doesNotExistsInParentChildren);
    }

    TEST(HierachySystem, DestroyEntityShouldRemoveChildrenAlso) {
        entt::registry registry;
        HierarchySystem hierarchySystem(registry);

        auto parent = registry.create();
        auto child = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        auto& childHierarchy = registry.emplace<Hierarchy>(child);

        childHierarchy.parent = parent;
        registry.patch<Hierarchy>(child);

        hierarchySystem.Update();

        registry.destroy(parent);

        EXPECT_FALSE(registry.valid(child));
    }

    TEST(HierachySystem, DestroyParentEntityShouldBeRemovedFromParentsChildrenList) {
        entt::registry registry;
        HierarchySystem hierarchySystem(registry);

        auto parent = registry.create();
        auto child = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        auto& childHierarchy = registry.emplace<Hierarchy>(child);

        childHierarchy.parent = parent;
        registry.patch<Hierarchy>(child);

        hierarchySystem.Update();

        registry.destroy(child);

        auto& parentChildren = registry.get<Hierarchy>(parent).children;
        bool doesNotExistsInParentChildren = std::find(parentChildren.begin(), parentChildren.end(), child)==parentChildren.end();
        EXPECT_TRUE(doesNotExistsInParentChildren);
    }


}