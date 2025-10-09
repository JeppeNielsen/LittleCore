//
// Created by Jeppe Nielsen on 05/10/2025.
//


#pragma once
#include "entt/entt.hpp"
#include <memory>
#include "EditorCamera.hpp"

namespace LittleCore {
    class EditorCameraController {
    public:
        using Cameras = std::vector<std::unique_ptr<EditorCamera>>;

        Cameras cameras;

        void CreateCamera();

        void Update(float dt);
    };
}