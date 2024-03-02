//
// Created by Jeppe Nielsen on 25/01/2024.
//

#include "gtest/gtest.h"
#include "RenderSystem.hpp"

using namespace LittleCore;

namespace {
    TEST(TestObservers, TestGroup) {

        struct Renderable {

        };

        struct Transform {

        };

        entt::registry registry;
        entt::observer observer {registry,
                                  entt::collector.group<Renderable, Transform>()};

        auto ent = registry.create();

        registry.emplace<Renderable>(ent);
        registry.emplace<Transform>(ent);

        EXPECT_EQ(1, observer.size());

        observer.clear();

        registry.patch<Renderable>(ent);

        EXPECT_EQ(0, observer.size());

        observer.clear();

        registry.patch<Transform>(ent);

        EXPECT_EQ(0, observer.size());

    }

    TEST(TestObservers, TestUpdate) {

        struct Renderable {

        };

        struct Transform {

        };

        entt::registry registry;
        entt::observer observer {registry,
                                 entt::collector
                                         .update<Renderable>().where<Transform>()
                                         .update<Transform>().where<Renderable>()};

        auto ent = registry.create();

        registry.emplace<Renderable>(ent);
        EXPECT_EQ(0, observer.size());

        registry.emplace<Transform>(ent);
        EXPECT_EQ(0, observer.size());

        registry.patch<Renderable>(ent);
        EXPECT_EQ(1, observer.size());

        observer.clear();

        registry.patch<Renderable>(ent);
        EXPECT_EQ(1, observer.size());

    }

    TEST(TestObservers, TestUpdateGroup) {

        struct Renderable {

        };

        struct Transform {

        };

        entt::registry registry;
        entt::observer observer {registry,
                                  entt::collector
                                          .update<Renderable>().where<Transform>()
                                          .update<Transform>().where<Renderable>()
                                          .group<Renderable, Transform>()};

        auto ent = registry.create();

        registry.emplace<Renderable>(ent);

        EXPECT_EQ(0, observer.size());

        observer.clear();

        registry.emplace<Transform>(ent);

        EXPECT_EQ(1, observer.size());


        registry.patch<Renderable>(ent);
        EXPECT_EQ(1, observer.size());

        observer.clear();

        registry.patch<Renderable>(ent);
        EXPECT_EQ(1, observer.size());

    }

    TEST(TestObservers, SingleGroup) {

        struct Component {

        };

        entt::registry registry;
        entt::observer observer {registry,
                                 entt::collector
                                         .update<Component>().group<Component>()};

        auto ent = registry.create();

        registry.emplace<Component>(ent);

        EXPECT_EQ(1, observer.size());

    }

}