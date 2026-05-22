//
// Created by Codex on 23/05/2026.
//

#include "gtest/gtest.h"

#include "Hierarchy.hpp"
#include "LayoutSystem.hpp"
#include "Layouter.hpp"
#include "LocalTransform.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

namespace {

    TEST(LayoutSystem, RectLayoutShouldPlaceChildUsingAnchorsPivotAndPosition) {
        entt::registry registry;
        LayoutSystem layoutSystem(registry);

        const auto parent = registry.create();
        registry.emplace<Hierarchy>(parent);
        registry.emplace<Sizable>(parent).size = {200.0f, 100.0f};

        const auto child = registry.create();
        auto& childHierarchy = registry.emplace<Hierarchy>(child);
        childHierarchy.parent = parent;
        registry.emplace<Sizable>(child);
        registry.emplace<LocalTransform>(child);

        auto& layouter = registry.emplace<Layouter>(child);
        layouter.anchorMin = {0.5f, 0.5f};
        layouter.anchorMax = {0.5f, 0.5f};
        layouter.anchoredPosition = {10.0f, -5.0f};
        layouter.sizeDelta = {80.0f, 20.0f};
        layouter.pivot = {0.5f, 0.5f};

        layoutSystem.Update();

        const auto& sizable = registry.get<Sizable>(child);
        const auto& transform = registry.get<LocalTransform>(child);
        EXPECT_EQ(sizable.size, vec2(80.0f, 20.0f));
        EXPECT_EQ(transform.position, vec3(70.0f, 35.0f, 0.0f));
    }

    TEST(LayoutSystem, RectLayoutShouldStretchWithinAnchorSpanUsingSizeDelta) {
        entt::registry registry;
        LayoutSystem layoutSystem(registry);

        const auto parent = registry.create();
        registry.emplace<Hierarchy>(parent);
        registry.emplace<Sizable>(parent).size = {300.0f, 200.0f};

        const auto child = registry.create();
        auto& childHierarchy = registry.emplace<Hierarchy>(child);
        childHierarchy.parent = parent;
        registry.emplace<Sizable>(child);
        registry.emplace<LocalTransform>(child);

        auto& layouter = registry.emplace<Layouter>(child);
        layouter.anchorMin = {0.0f, 0.0f};
        layouter.anchorMax = {1.0f, 1.0f};
        layouter.sizeDelta = {-20.0f, -40.0f};
        layouter.pivot = {0.5f, 0.5f};

        layoutSystem.Update();

        const auto& sizable = registry.get<Sizable>(child);
        const auto& transform = registry.get<LocalTransform>(child);
        EXPECT_EQ(sizable.size, vec2(280.0f, 160.0f));
        EXPECT_EQ(transform.position, vec3(10.0f, 20.0f, 0.0f));
    }

    TEST(LayoutSystem, DefaultLayouterShouldPreserveCurrentRect) {
        entt::registry registry;
        LayoutSystem layoutSystem(registry);

        const auto parent = registry.create();
        registry.emplace<Hierarchy>(parent);
        registry.emplace<Sizable>(parent).size = {200.0f, 120.0f};

        const auto child = registry.create();
        auto& childHierarchy = registry.emplace<Hierarchy>(child);
        childHierarchy.parent = parent;
        auto& sizable = registry.emplace<Sizable>(child);
        sizable.size = {42.0f, 17.0f};
        auto& transform = registry.emplace<LocalTransform>(child);
        transform.position = {3.0f, 4.0f, 5.0f};
        registry.emplace<Layouter>(child);

        layoutSystem.Update();

        EXPECT_EQ(sizable.size, vec2(42.0f, 17.0f));
        EXPECT_EQ(transform.position, vec3(3.0f, 4.0f, 5.0f));
    }

    TEST(LayoutSystem, DefaultLayouterShouldInitializeFromCurrentRectBeforeApplying) {
        entt::registry registry;
        LayoutSystem layoutSystem(registry);

        const auto parent = registry.create();
        registry.emplace<Hierarchy>(parent);
        registry.emplace<Sizable>(parent).size = {200.0f, 120.0f};

        const auto child = registry.create();
        auto& childHierarchy = registry.emplace<Hierarchy>(child);
        childHierarchy.parent = parent;
        auto& sizable = registry.emplace<Sizable>(child);
        sizable.size = {40.0f, 30.0f};
        auto& transform = registry.emplace<LocalTransform>(child);
        transform.position = {12.0f, 18.0f, 0.0f};
        auto& layouter = registry.emplace<Layouter>(child);

        layoutSystem.Update();

        EXPECT_EQ(layouter.anchorMin, vec2(0.0f, 0.0f));
        EXPECT_EQ(layouter.anchorMax, vec2(0.0f, 0.0f));
        EXPECT_EQ(layouter.sizeDelta, vec2(40.0f, 30.0f));
        EXPECT_EQ(layouter.pivot, vec2(0.5f, 0.5f));
        EXPECT_EQ(layouter.anchoredPosition, vec2(32.0f, 33.0f));
        EXPECT_EQ(sizable.size, vec2(40.0f, 30.0f));
        EXPECT_EQ(transform.position, vec3(12.0f, 18.0f, 0.0f));
    }

    TEST(LayoutSystem, RectLayoutShouldResolveNestedHierarchyTopDown) {
        entt::registry registry;
        LayoutSystem layoutSystem(registry);

        const auto root = registry.create();
        registry.emplace<Hierarchy>(root);
        registry.emplace<Sizable>(root).size = {300.0f, 200.0f};

        const auto container = registry.create();
        auto& containerHierarchy = registry.emplace<Hierarchy>(container);
        containerHierarchy.parent = root;
        registry.emplace<Sizable>(container);
        registry.emplace<LocalTransform>(container);
        auto& containerLayouter = registry.emplace<Layouter>(container);
        containerLayouter.anchorMin = {0.0f, 0.0f};
        containerLayouter.anchorMax = {0.0f, 0.0f};
        containerLayouter.anchoredPosition = {40.0f, 30.0f};
        containerLayouter.sizeDelta = {160.0f, 120.0f};
        containerLayouter.pivot = {0.0f, 0.0f};

        const auto grandchild = registry.create();
        auto& grandchildHierarchy = registry.emplace<Hierarchy>(grandchild);
        grandchildHierarchy.parent = container;
        registry.emplace<Sizable>(grandchild);
        registry.emplace<LocalTransform>(grandchild);
        auto& grandchildLayouter = registry.emplace<Layouter>(grandchild);
        grandchildLayouter.anchorMin = {1.0f, 1.0f};
        grandchildLayouter.anchorMax = {1.0f, 1.0f};
        grandchildLayouter.anchoredPosition = {-5.0f, -7.0f};
        grandchildLayouter.sizeDelta = {20.0f, 10.0f};
        grandchildLayouter.pivot = {1.0f, 1.0f};

        layoutSystem.Update();

        EXPECT_EQ(registry.get<Sizable>(container).size, vec2(160.0f, 120.0f));
        EXPECT_EQ(registry.get<LocalTransform>(container).position, vec3(40.0f, 30.0f, 0.0f));
        EXPECT_EQ(registry.get<Sizable>(grandchild).size, vec2(20.0f, 10.0f));
        EXPECT_EQ(registry.get<LocalTransform>(grandchild).position, vec3(135.0f, 103.0f, 0.0f));
    }

}
