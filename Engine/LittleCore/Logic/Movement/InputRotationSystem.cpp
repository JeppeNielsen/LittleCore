//
// Created by Jeppe Nielsen on 27/09/2025.
//

#include "InputRotationSystem.hpp"
#include "LocalTransform.hpp"
#include "Input.hpp"
#include "InputRotation.hpp"

using namespace LittleCore;

void InputRotationSystem::Update(float dt) {

    auto view = registry.view<LocalTransform, const Input, InputRotation>();

    for(auto[entity, transform, input, movement] : view.each()) {

        if (!movement.isRotating && input.IsTouchDown({movement.touchIndex})) {
            movement.isRotating = true;
            movement.startingRotation = transform.rotation;
            movement.touchPosition = input.touchPosition[0].position;
        } else if (movement.isRotating && input.IsTouchUp({movement.touchIndex})) {
            movement.isRotating = false;
        }

        if (movement.isRotating) {

            vec2 delta = movement.touchPosition - input.touchPosition[0].position;

            auto pitch = glm::pitch(movement.startingRotation);
            auto yaw = glm::yaw(movement.startingRotation);
            yaw -= delta.x * movement.rotationSpeed;
            pitch -= delta.y * movement.rotationSpeed;

            const float piHalf = glm::pi<float>() * 0.5f;

            if (pitch<-piHalf) {
                pitch = -piHalf;
            } else if (pitch>piHalf) {
                pitch = piHalf;
            }

            transform.rotation = glm::quat({pitch, yaw, 0});
            registry.patch<LocalTransform>(entity);
        }
    }

}
