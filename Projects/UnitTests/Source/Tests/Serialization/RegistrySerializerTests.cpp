//
// Created by Jeppe Nielsen on 16/03/2025.
//

#include "gtest/gtest.h"
#include "Hierarchy.hpp"
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
using HierarchySerializer = RegistrySerializer<Hierarchy>;

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

    TEST(RegistrySerializer, HierarchyChildrenOrderShouldRoundTrip) {
        entt::registry registry;

        const auto parent = registry.create();
        const auto childA = registry.create();
        const auto childB = registry.create();
        const auto childC = registry.create();

        auto& parentHierarchy = registry.emplace<Hierarchy>(parent);
        parentHierarchy.children = {childB, childA, childC};

        auto& childAHierarchy = registry.emplace<Hierarchy>(childA);
        childAHierarchy.parent = parent;
        childAHierarchy.previousParent = parent;

        auto& childBHierarchy = registry.emplace<Hierarchy>(childB);
        childBHierarchy.parent = parent;
        childBHierarchy.previousParent = parent;

        auto& childCHierarchy = registry.emplace<Hierarchy>(childC);
        childCHierarchy.parent = parent;
        childCHierarchy.previousParent = parent;

        HierarchySerializer serializer;
        const auto serializedString = serializer.Serialize(registry);
        EXPECT_EQ(serializedString.find("previousParent"), std::string::npos);

        entt::registry deserializedRegistry;
        ASSERT_TRUE(serializer.Deserialize(deserializedRegistry, serializedString).empty());

        const auto& deserializedHierarchy = deserializedRegistry.get<Hierarchy>(parent);
        ASSERT_EQ(deserializedHierarchy.children.size(), 3);
        EXPECT_EQ(deserializedHierarchy.children[0], childB);
        EXPECT_EQ(deserializedHierarchy.children[1], childA);
        EXPECT_EQ(deserializedHierarchy.children[2], childC);

        const auto& deserializedChildHierarchy = deserializedRegistry.get<Hierarchy>(childA);
        EXPECT_EQ(deserializedChildHierarchy.parent, parent);
        EXPECT_EQ(deserializedChildHierarchy.previousParent, parent);
    }

}
