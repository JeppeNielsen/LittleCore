//
// Created by Jeppe Nielsen on 16/03/2025.
//

#include "gtest/gtest.h"
#include "RegistrySerializer.hpp"

using namespace LittleCore;

struct Transform {
    float x;
    float y;
};

struct Velocity {
    float vx;
    float vy;
};

using Serializer = RegistrySerializer<Transform, Velocity>;

namespace {

    TEST(RegistrySerializer, PtrOperatorShouldReturnBaseValuesOnInvalidResourceHandle) {


        entt::registry registry;
        auto entity = registry.create();
        auto& transform = registry.emplace<Transform>(entity);
        transform.x = 123;
        transform.y = 456;

        Serializer serializer;

        auto serializedString = serializer.Serialize(registry);

        entt::registry deserializedRegistry;
        serializer.Deserialize(deserializedRegistry, serializedString);
        auto view = deserializedRegistry.view<Transform>();

        for(auto e : view) {
            auto t = deserializedRegistry.get<Transform>(e);
            EXPECT_EQ(t.x, 123);
            EXPECT_EQ(t.y, 456);
        }

    }





}
