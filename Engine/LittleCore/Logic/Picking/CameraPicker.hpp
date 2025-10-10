//
// Created by Jeppe Nielsen on 09/10/2025.
//

#pragma once
#include "entt/entt.hpp"
#include <vector>

namespace LittleCore {
    struct CameraPicker {
        bool hasPicked = false;
        std::vector<entt::entity> pickedEntities;
    };
}