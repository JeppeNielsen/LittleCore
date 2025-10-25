//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "gtest/gtest.h"
#include "RegistryHelper.hpp"
#include "Hierarchy.hpp"

using namespace LittleCore;

namespace {

    struct Position {
        int x = 0;
        int y = 0;
    };

    struct Velocity {
        int vx = 0;
        int vy = 0;
    };

    TEST(RegistryHelper, Duplicate) {

        entt::registry srcRegistry;
        entt::registry destRegistry;
        destRegistry.assure<Hierarchy>();
        destRegistry.assure<Position>();


        auto srcEntity = srcRegistry.create();
        srcRegistry.emplace<Hierarchy>(srcEntity);
        auto& srcPosition = srcRegistry.emplace<Position>(srcEntity);
        srcPosition.x = 123;
        srcPosition.y = 456;

        auto copy = RegistryHelper::Duplicate(srcRegistry, srcEntity, destRegistry);

        auto& copyPosition = destRegistry.get<Position>(copy);


        ASSERT_TRUE(srcPosition.x == copyPosition.x);
    }

}