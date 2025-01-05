//
// Created by Jeppe Nielsen on 22/12/2024.
//

#pragma once
#include <entt/entt.hpp>
#include "TypeUtility.hpp"
#include <imgui.h>

template<typename Component>
struct ComponentEditor {
public:
    virtual ~ComponentEditor() = default;

    bool HasComponent(entt::registry& registry, entt::entity entity) {
        return registry.any_of<Component>(entity);
    }

    void TryDraw(entt::registry& registry, entt::entity entity) {
        if (!HasComponent(registry, entity)) {
            return;
        }

        Component& component = registry.get<Component>(entity);
        ImGui::PushID(&component);
        if (Draw(registry, entity, component)) {
            registry.patch<Component>(entity);
        }
        ImGui::PopID();
    }

    std::string GetId() {
        return LittleCore::TypeUtility::GetClassName<Component>();
    }

    virtual bool Draw(entt::registry& registry, entt::entity entity, Component& component) = 0;

    virtual void Create(entt::registry& registry, entt::entity entity) {
        registry.emplace<Component>(entity);
    }
};
