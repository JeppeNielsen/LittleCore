//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once
#include "entt/entt.hpp"
#include "ResizableFrameBuffer.hpp"


namespace LittleCore {
    class EditorCamera {
    public:

        EditorCamera();

        entt::registry registry;
        entt::entity cameraEntity;
        ResizableFrameBuffer frameBuffer;
    };
}
