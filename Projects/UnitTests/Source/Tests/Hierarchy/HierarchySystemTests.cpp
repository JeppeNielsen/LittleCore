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

        auto& parentChildren = parentHierarchy.children;
        bool doesNotExistsInParentChildren = std::find(parentChildren.begin(), parentChildren.end(), child)==parentChildren.end();
        EXPECT_TRUE(doesNotExistsInParentChildren);
    }

    TEST(HierachySystem, HierarchyChildrenShouldNotBeEmptyWhenCopiedFromMoreEntities) {
        entt::registry registry;
        auto root = registry.create();
        Hierarchy& hierarchy = registry.emplace<Hierarchy>(root);
        hierarchy.children.push_back(root);
        hierarchy.children.push_back(root);

        for (int i = 0; i < 100000; ++i) {
            auto child = registry.create();
            registry.emplace<Hierarchy>(child);
        }

        Hierarchy& hierarchyAfter = registry.get<Hierarchy>(root);
        EXPECT_EQ(hierarchyAfter.children.size(), 2);


        std::vector<Hierarchy> list;
        Hierarchy first;
        first.children.push_back(root);
        first.children.push_back(root);
        list.push_back(first);

        for (int i = 0; i < 10000; ++i) {
            list.push_back({});
        }

        EXPECT_EQ(list[0].children.size(), 2);
    }


}