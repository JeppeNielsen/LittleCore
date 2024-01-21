//
// Created by Jeppe Nielsen on 21/01/2024.
//


#include "gtest/gtest.h"
#include "MeshBoundingBoxSystem.hpp"

using namespace LittleCore;

namespace {
    TEST(MeshBoundingBoxSystem, MeshBoundingBoxSystemTests) {
        entt::registry registry;
        MeshBoundingBoxSystem meshBoundingBoxSystem(registry);

        auto entity = registry.create();

        auto &localBoundingBox = registry.emplace<LocalBoundingBox>(entity);
        auto &mesh = registry.emplace<Mesh>(entity);

        mesh.vertices.push_back({{0, 0, 0}, 0, {0, 0}});
        mesh.vertices.push_back({{0, 1, 0}, 0, {0, 0}});
        mesh.vertices.push_back({{1, 1, 0}, 0, {0, 0}});
        mesh.vertices.push_back({{1, 0, 0}, 0, {0, 0}});

        registry.patch<Mesh>(entity);

        meshBoundingBoxSystem.Update();

        EXPECT_EQ(localBoundingBox.bounds.center, vec3( 0.5f, 0.5f, 0 ));
    }
}