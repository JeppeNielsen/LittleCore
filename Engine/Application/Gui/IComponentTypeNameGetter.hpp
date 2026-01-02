//
// Created by Jeppe Nielsen on 03/11/2025.
//

#pragma once
#include <vector>
#include <string>
#include <entt/entt.hpp>

namespace LittleCore {
    struct IComponentTypeNameGetter {
        virtual std::vector <std::string> GetComponentTypeNames(entt::registry& registry, entt::entity entity) = 0;
    };
}
