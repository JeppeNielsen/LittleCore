//
// Created by Jeppe Nielsen on 20/01/2024.
//
#include <Hierarchy.hpp>

#include "gtest/gtest.h"
#include "OctreeSystem.hpp"
#include "WorldBoundingBox.hpp"

using namespace LittleCore;

namespace {
    TEST(OctreeSystem, OctreeSystemTests) {
        entt::registry registry;
        OctreeSystem<> octreeSystem(registry);

        auto entity = registry.create();

        auto& worldBoundingBox = registry.emplace<WorldBoundingBox>(entity);
        worldBoundingBox.bounds = {{0,0,0} , {1,1,1}};

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
}