//
// Created by Jeppe Nielsen on 04/11/2025.
//

#pragma once
#include <string>
#include <entt/entt.hpp>

namespace LittleCore {
    struct IComponentGuiDrawer {
        ~IComponentGuiDrawer() = default;
        virtual void DrawComponent(std::string& componentTypeName, entt::registry& registry, entt::entity entity) = 0;
    };
}
