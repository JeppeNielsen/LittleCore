//
// Created by Jeppe Nielsen on 16/01/2024.
//

#pragma once
#include "Math.hpp"

namespace LittleCore {
    struct WorldTransform {
        glm::mat4x4 world;
        glm::mat4x4 worldInverse;
    };
}