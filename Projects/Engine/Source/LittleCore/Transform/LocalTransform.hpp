//
// Created by Jeppe Nielsen on 15/01/2024.
//

#pragma once
#include "Math.hpp"
#include <glm/gtx/quaternion.hpp>

namespace LittleCore {
    struct LocalTransform {
        glm::vec3 position = {0.0f,0.0f,0.0f};
        glm::quat rotation = quat(1.0f,0.0f,0.0f,0.0f);
        glm::vec3 scale = vec3(1.0f,1.0f,1.0f);

        mat4 GetLocalToParent() const {
            return glm::scale(glm::translate(mat4x4(1.0f), position), scale) * glm::toMat4(rotation);
        }
    };


}
