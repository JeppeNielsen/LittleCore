//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "gtest/gtest.h"
#include "RegistryHelper.hpp"
#include "Hierarchy.hpp"
#include "Prefab.hpp"
#include "HierarchySystem.hpp"

using namespace LittleCore;

namespace {

    struct Position {
        int x = 0;
        int y = 0;
    };

    TEST(RegistryHelper, DuplicateOneEntityComponent_ShouldBeSameValue) {

        entt::registry srcRegistry;
        entt::registry destRegistry;

        auto srcEntity = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcEntity);
        auto& srcPosition = srcRegistry.emplace<Position>(srcEntity);
        srcPosition.x = 123;
        srcPosition.y = 456;

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcEntity, destRegistry);

        auto& copyPosition = destRegistry.get<Position>(copy);

        ASSERT_TRUE(srcPosition.x == copyPosition.x);
    }

    TEST(RegistryHelper, DuplicateOneEntityWithOneChild_ShouldKeepHierarchy) {

        entt::registry srcRegistry;
        entt::registry destRegistry;


        auto srcRoot = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcRoot);
        srcRegistry.emplace<Position>(srcRoot) = {123, 456 };

        auto srcChild = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcChild).parent = srcRoot;
        srcRegistry.emplace<Position>(srcChild) = {666,777};

        srcRegistry.get<Hierarchy>(srcRoot).children.push_back(srcChild);

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcRoot, destRegistry);

        auto& copyHierarchy = destRegistry.get<Hierarchy>(copy);
        auto& copyPosition = destRegistry.get<Position>(copy);
        auto& childPosition = destRegistry.get<Position>(copyHierarchy.children[0]);

        ASSERT_TRUE(copyPosition.x == 123);
        ASSERT_TRUE(copyPosition.y == 456);

        ASSERT_TRUE(childPosition.x == 666);
        ASSERT_TRUE(childPosition.y == 777);
    }

    TEST(RegistryHelper, DuplicateOneEntityWithPrefabRootShouldBeNull) {

        entt::registry srcRegistry;
        entt::registry destRegistry;

        auto srcRoot = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcRoot);
        srcRegistry.emplace<Prefab>(srcRoot);

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcRoot, destRegistry);

        auto& copyHierarchy = destRegistry.get<Hierarchy>(copy);
        auto& prefab = destRegistry.get<Prefab>(copy);

        ASSERT_TRUE(prefab.roots.empty());
    }

    TEST(RegistryHelper, DuplicateOneEntityWithOneChildDuplicatedShouldHaveOneChildInChildrenList) {

        entt::registry srcRegistry;
        HierarchySystem srcHierarchySystem(srcRegistry);

        entt::registry destRegistry;
        HierarchySystem destHierarchySystem(destRegistry);

        auto srcRoot = srcRegistry.create();
        auto& srcRootHierarchy = srcRegistry.emplace<Hierarchy>(srcRoot);
        auto srcChild = srcRegistry.create();
        auto& srcChildHierarchy = srcRegistry.emplace<Hierarchy>(srcChild);
        srcChildHierarchy.parent = srcRoot;
        srcHierarchySystem.Update();

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcRoot, destRegistry);

        auto& copyBeforeHierarchy = destRegistry.get<Hierarchy>(copy);

        destHierarchySystem.Update();

        auto& copyHierarchy = destRegistry.get<Hierarchy>(copy);

        ASSERT_EQ(copyBeforeHierarchy.children.size(), 1);
        ASSERT_EQ(copyHierarchy.children.size(), 1);
    }

}