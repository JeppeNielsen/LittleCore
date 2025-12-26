//
// Created by Jeppe Nielsen on 30/10/2025.
//

#pragma once
#include <entt/entt.hpp>
#include <vector>
#include <string>


namespace LittleCore {
    struct SerializedPrefabComponent {
        entt::entity sourceEntity;
        entt::entity entity;
        std::string componentId;
        std::string data;
    };
}

