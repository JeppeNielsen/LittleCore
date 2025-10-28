//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "gtest/gtest.h"
#include "RegistryHelper.hpp"
#include "Hierarchy.hpp"
#include "PrefabInstance.hpp"

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

    TEST(RegistryHelper, DuplicateOneEntityWithPrefabInstanceRootShouldBeNull) {

        entt::registry srcRegistry;
        entt::registry destRegistry;

        auto srcRoot = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcRoot);
        srcRegistry.emplace<PrefabInstance>(srcRoot);

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcRoot, destRegistry);

        auto& copyHierarchy = destRegistry.get<Hierarchy>(copy);
        auto& prefabInstance = destRegistry.get<PrefabInstance>(copy);

        ASSERT_TRUE(prefabInstance.roots.empty());
    }

}