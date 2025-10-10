//
// Created by Jeppe Nielsen on 09/10/2025.
//

#include "CameraPickerSystem.hpp"
#include "WorldTransform.hpp"
#include "Camera.hpp"
#include "CameraPicker.hpp"
#include "Input.hpp"

using namespace LittleCore;

CameraPickerSystem::CameraPickerSystem(entt::registry& registry, PickingSystem<>& pickingSystem)
: SystemBase(registry), pickingSystem(pickingSystem) {

}

void CameraPickerSystem::Update() {
    for(auto[entity, worldTransform, camera, input, picker] : registry.view<const WorldTransform, const Camera, const Input, CameraPicker>().each()) {
        picker.pickedEntities.clear();
        picker.hasPicked = false;
        if (input.IsTouchDown({0})) {
            ivec2 screenPos = input.touchPosition[0].position;
            auto ray = camera.GetRay(worldTransform, input.screenSize, screenPos);
            picker.pickedEntities = pickingSystem.Pick(ray);
            picker.hasPicked = true;
        }
    }
}
