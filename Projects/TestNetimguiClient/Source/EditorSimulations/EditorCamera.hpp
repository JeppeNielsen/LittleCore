//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once

#include "MovableSystem.hpp"
#include "InputRotationSystem.hpp"
#include "entt/entt.hpp"
#include "ResizableFrameBuffer.hpp"
#include "DefaultSimulation.hpp"

namespace LittleCore {
    class EditorCamera {
    public:

        EditorCamera();

        entt::entity cameraEntity;
        ResizableFrameBuffer frameBuffer;
        CustomSimulation<MovableSystem, InputRotationSystem> simulation;

    };
}
