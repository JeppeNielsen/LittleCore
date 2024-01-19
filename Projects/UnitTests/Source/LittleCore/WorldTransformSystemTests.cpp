//
// Created by Jeppe Nielsen on 19/01/2024.
//

#include <Hierarchy.hpp>

#include "gtest/gtest.h"
#include "WorldTransformSystem.hpp"

using namespace LittleCore;

namespace {
    TEST(WorldTransformSystem, HierarchyPatchedShouldCalculateCorrectWorldTransform) {
        entt::registry registry;
        WorldTransformSystem worldTransformSystem(registry);

        auto parent = registry.create();
        auto child = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        auto& parentLocalTransform = registry.emplace<LocalTransform>(parent);
        auto& parentWorldTransform = registry.emplace<WorldTransform>(parent);

        auto& childHierarchy = registry.emplace<Hierarchy>(child);
        auto& childLocalTransform = registry.emplace<LocalTransform>(child);
        auto& childWorldTransform = registry.emplace<WorldTransform>(child);

        parentLocalTransform.position = {1,0,0};
        childLocalTransform.position = {1,0,0};

        parentHierarchy.children.push_back(child);
        childHierarchy.parent = parent;
        registry.patch<Hierarchy>(parent);
        registry.patch<Hierarchy>(child);

        worldTransformSystem.Update();

        vec4 vec = {0,0,0,1};
        vec = childWorldTransform.world * vec;

        EXPECT_EQ(vec.x, 2);

        vec4 vecInverse = {0,0,0,1};
        vecInverse = childWorldTransform.worldInverse * vecInverse;

        EXPECT_EQ(vecInverse.x, -2);

        parentLocalTransform.position = {2,0,0};
        registry.patch<LocalTransform>(parent);

        worldTransformSystem.Update();

        vec = {0,0,0,1};
        vec = childWorldTransform.world * vec;

        EXPECT_EQ(vec.x, 3);

        vecInverse = {0,0,0,1};
        vecInverse = childWorldTransform.worldInverse * vecInverse;

        EXPECT_EQ(vecInverse.x, -3);

        childHierarchy.parent = entt::null;
        parentHierarchy.children.clear();

        registry.patch<Hierarchy>(child);

        worldTransformSystem.Update();

        vec = {0,0,0,1};
        vec = childWorldTransform.world * vec;

        EXPECT_EQ(vec.x, 1);

        vecInverse = {0,0,0,1};
        vecInverse = childWorldTransform.worldInverse * vecInverse;

        EXPECT_EQ(vecInverse.x, -1);
    }
}