//
// Created by Jeppe Nielsen on 16/11/2024.
//


#pragma once
#include "SystemBase.hpp"
#include "Movable.hpp"

namespace LittleCore {
    struct MovableSystem : SystemBase {
        void Update(float dt);
    };
}


