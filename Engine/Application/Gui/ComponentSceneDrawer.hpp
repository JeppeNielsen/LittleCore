//
// Created by Codex on 02/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "SceneDrawerContext.hpp"

namespace LittleCore {

    template<typename Component>
    struct ComponentSceneDrawer {
    public:
        virtual ~ComponentSceneDrawer() = default;

        bool HasComponent(entt::registry& registry, entt::entity entity) {
            return registry.any_of<Component>(entity);
        }

        void TryDraw(SceneDrawerContext& context) {
            if (!HasComponent(context.objectRegistry, context.objectEntity)) {
                return;
            }

            Component& component = context.objectRegistry.get<Component>(context.objectEntity);
            Draw(context, component);
        }

        virtual void Draw(SceneDrawerContext& context, Component& component) = 0;
    };
}
