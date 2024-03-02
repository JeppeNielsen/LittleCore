//
// Created by Jeppe Nielsen on 20/01/2024.
//
#include "gtest/gtest.h"
#include "WorldBoundingBoxSystem.hpp"

using namespace LittleCore;

namespace {
    TEST(WorldBoundingBoxSystem, WorldBoundingBoxSystemTests) {
        entt::registry registry;
        WorldBoundingBoxSystem worldBoundingBoxSystem(registry);

        auto entity = registry.create();

        auto& worldTransform = registry.emplace<WorldTransform>(entity);
        auto& localBoundingBox = registry.emplace<LocalBoundingBox>(entity);
        auto& worldBoundingBox = registry.emplace<WorldBoundingBox>(entity);
        localBoundingBox.bounds = {{0,0,0} , {1,1,1}};

        worldTransform.world = glm::translate(mat4x4(1.0f), {3,0,0});

        registry.patch<WorldTransform>(entity);

        worldBoundingBoxSystem.Update();

        EXPECT_EQ(worldBoundingBox.bounds.center.x, 3);

    }


}