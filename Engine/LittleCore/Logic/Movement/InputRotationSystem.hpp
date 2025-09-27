//
// Created by Jeppe Nielsen on 27/09/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "InputRotation.hpp"

namespace LittleCore {
    struct InputRotationSystem : SystemBase {
        void Update(float dt);
    };
}
