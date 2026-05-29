//
// Created by Codex on 25/05/2026.
//

#include "gtest/gtest.h"

#include "Mesh.hpp"
#include "Sizable.hpp"
#include "Sprite.hpp"
#include "SpriteMeshSystem.hpp"

using namespace LittleCore;

namespace {

    void ExpectVertex(const Vertex& vertex, const vec3& position, const vec2& uv) {
        EXPECT_EQ(vertex.position, position);
        EXPECT_EQ(vertex.color, 0xFFFFFFFF);
        EXPECT_EQ(vertex.uv, uv);
    }

    TEST(SpriteMeshSystem, ZeroSlicingShouldCreateSingleQuad) {
        entt::registry registry;
        SpriteMeshSystem spriteMeshSystem(registry);

        const auto entity = registry.create();
        auto& sizable = registry.emplace<Sizable>(entity);
        sizable.size = {100.0f, 50.0f};
        registry.emplace<Mesh>(entity);
        auto& sprite = registry.emplace<Sprite>(entity);
        sprite.cornerSize = {20.0f, 10.0f};

        spriteMeshSystem.Update();

        const auto& mesh = registry.get<Mesh>(entity);
        ASSERT_EQ(mesh.vertices.size(), 4);
        ASSERT_EQ(mesh.triangles.size(), 6);

        ExpectVertex(mesh.vertices[0], {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f});
        ExpectVertex(mesh.vertices[1], {100.0f, 0.0f, 0.0f}, {1.0f, 0.0f});
        ExpectVertex(mesh.vertices[2], {100.0f, 50.0f, 0.0f}, {1.0f, 1.0f});
        ExpectVertex(mesh.vertices[3], {0.0f, 50.0f, 0.0f}, {0.0f, 1.0f});

        EXPECT_EQ(mesh.triangles, std::vector<uint16_t>({0, 1, 2, 0, 2, 3}));
    }

    TEST(SpriteMeshSystem, EdgePercentCutsAndCornerSizeShouldCreateSlicedMesh) {
        entt::registry registry;
        SpriteMeshSystem spriteMeshSystem(registry);

        const auto entity = registry.create();
        auto& sizable = registry.emplace<Sizable>(entity);
        sizable.size = {20.0f, 20.0f};
        registry.emplace<Mesh>(entity);
        auto& sprite = registry.emplace<Sprite>(entity);
        sprite.slicingLeft = 0.3f;
        sprite.slicingRight = 0.3f;
        sprite.slicingBottom = 0.3f;
        sprite.slicingTop = 0.3f;
        sprite.cornerSize = {2.0f, 2.0f};

        spriteMeshSystem.Update();

        const auto& mesh = registry.get<Mesh>(entity);
        ASSERT_EQ(mesh.vertices.size(), 16);
        ASSERT_EQ(mesh.triangles.size(), 54);

        ExpectVertex(mesh.vertices[0], {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f});
        ExpectVertex(mesh.vertices[1], {2.0f, 0.0f, 0.0f}, {0.3f, 0.0f});
        ExpectVertex(mesh.vertices[2], {18.0f, 0.0f, 0.0f}, {0.7f, 0.0f});
        ExpectVertex(mesh.vertices[3], {20.0f, 0.0f, 0.0f}, {1.0f, 0.0f});
        ExpectVertex(mesh.vertices[4], {0.0f, 2.0f, 0.0f}, {0.0f, 0.3f});
        ExpectVertex(mesh.vertices[5], {2.0f, 2.0f, 0.0f}, {0.3f, 0.3f});
        ExpectVertex(mesh.vertices[10], {18.0f, 18.0f, 0.0f}, {0.7f, 0.7f});
        ExpectVertex(mesh.vertices[15], {20.0f, 20.0f, 0.0f}, {1.0f, 1.0f});

        EXPECT_EQ(mesh.triangles, std::vector<uint16_t>({
                0, 1, 5, 0, 5, 4,
                1, 2, 6, 1, 6, 5,
                2, 3, 7, 2, 7, 6,
                4, 5, 9, 4, 9, 8,
                5, 6, 10, 5, 10, 9,
                6, 7, 11, 6, 11, 10,
                8, 9, 13, 8, 13, 12,
                9, 10, 14, 9, 14, 13,
                10, 11, 15, 10, 15, 14
        }));
    }

}
