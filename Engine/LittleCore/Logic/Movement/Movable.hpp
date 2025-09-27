//
// Created by Jeppe Nielsen on 16/11/2024.
//


#pragma once
#include "InputKey.hpp"
#include "Math.hpp"
#include <vector>

namespace LittleCore {
    struct MovementKey {
        InputKey key;
        vec3 direction;
        bool isActive;
    };

    struct Movable {
        float speed = 1.0f;
        std::vector<MovementKey> keys;
    };
}