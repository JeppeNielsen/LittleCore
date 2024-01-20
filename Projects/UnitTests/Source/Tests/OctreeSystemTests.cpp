//
// Created by Jeppe Nielsen on 20/01/2024.
//
#include "gtest/gtest.h"
#include "OctreeSystem.hpp"
#include "WorldBoundingBox.hpp"

using namespace LittleCore;

namespace {
    TEST(OctreeSystem, OctreeSystemRayQueryTests) {
        entt::registry registry;
        OctreeSystem<> octreeSystem(registry);

        auto entity = registry.create();

        auto& worldBoundingBox = registry.emplace<WorldBoundingBox>(entity);
        worldBoundingBox.bounds = {{0,0,0} , {1,1,1}};

        registry.patch<WorldBoundingBox>(entity);

        octreeSystem.Update();

        Ray ray = {{0,0,-2},{0,0,1}};

        std::vector<entt::entity> entitiesInQuery;

        octreeSystem.Query(ray, entitiesInQuery);

        EXPECT_EQ(entitiesInQuery.size(), 1);

        entitiesInQuery.clear();

        ray = {{0,0,-2},{2,0,1}};

        octreeSystem.Query(ray, entitiesInQuery);

        EXPECT_EQ(entitiesInQuery.size(), 0);
    }

    TEST(OctreeSystem, OctreeSystemBoundingBoxQueryTests) {
        entt::registry registry;
        OctreeSystem<> octreeSystem(registry);

        auto entity = registry.create();

        auto& worldBoundingBox = registry.emplace<WorldBoundingBox>(entity);
        worldBoundingBox.bounds = {{0,0,0} , {1,1,1}};

        registry.patch<WorldBoundingBox>(entity);

        octreeSystem.Update();

        BoundingBox boundingBox = {{1,0,0},{1,1,1}};

        std::vector<entt::entity> entitiesInQuery;

        octreeSystem.Query(boundingBox, entitiesInQuery);

        EXPECT_EQ(entitiesInQuery.size(), 1);

        entitiesInQuery.clear();

        boundingBox = {{4,0,0},{1,1,1}};

        octreeSystem.Query(boundingBox, entitiesInQuery);

        EXPECT_EQ(entitiesInQuery.size(), 0);
    }
}