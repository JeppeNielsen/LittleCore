//
// Created by Jeppe Nielsen on 22/12/2024.
//

#pragma once
#include <entt/entt.hpp>

template<typename Component>
struct ComponentEditor {
public:
    virtual ~ComponentEditor() = default;

    void TryDraw(entt::registry& registry, entt::entity entity) {

        if (!registry.any_of<Component>(entity)) {
            return;
        }
        Component& component = registry.get<Component>(entity);
        Draw(registry, entity, component);
    }

    virtual void Draw(entt::registry& registry, entt::entity entity, Component& component) = 0;
};
