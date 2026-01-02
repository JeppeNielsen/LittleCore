//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "EditorCameraController.hpp"
#include "LocalTransform.hpp"
#include "Camera.hpp"

using namespace LittleCore;

void EditorCameraController::CreateCamera(PickingSystem<>& pickingSystem) {
    cameras.push_back(std::make_unique<EditorCamera>(pickingSystem));
}

void EditorCameraController::Update(float dt) {
    for(auto& camera : cameras) {
        camera->Update(dt);
    }
}
