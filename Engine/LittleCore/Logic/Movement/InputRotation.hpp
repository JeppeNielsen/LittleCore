//
// Created by Jeppe Nielsen on 27/09/2025.
//

#pragma once
#include "Math.hpp"

namespace LittleCore {
    struct InputRotation {
        int touchIndex = 0;
        float rotationSpeed = 0.001f;
        bool isRotating = false;

        quat startingRotation;
        vec2 touchPosition;
    };
}