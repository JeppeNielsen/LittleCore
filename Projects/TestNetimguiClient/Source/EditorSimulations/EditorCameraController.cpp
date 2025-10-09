//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "EditorCameraController.hpp"
#include "LocalTransform.hpp"
#include "Camera.hpp"

using namespace LittleCore;

void EditorCameraController::CreateCamera() {
    cameras.push_back(std::make_unique<EditorCamera>());
}

void EditorCameraController::Update(float dt) {
    for(auto& camera : cameras) {
        camera->simulation.Update(dt);
    }
}
