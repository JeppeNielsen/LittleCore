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

        glm::mat4 GetLocalToParent() const {
            glm::mat3 rotationMatrix = glm::mat3_cast(rotation);
            glm::mat4 result(1.0f);

            result[0][0] = rotationMatrix[0][0] * scale.x;
            result[0][1] = rotationMatrix[0][1] * scale.x;
            result[0][2] = rotationMatrix[0][2] * scale.x;

            result[1][0] = rotationMatrix[1][0] * scale.y;
            result[1][1] = rotationMatrix[1][1] * scale.y;
            result[1][2] = rotationMatrix[1][2] * scale.y;

            result[2][0] = rotationMatrix[2][0] * scale.z;
            result[2][1] = rotationMatrix[2][1] * scale.z;
            result[2][2] = rotationMatrix[2][2] * scale.z;

            result[3][0] = position.x;
            result[3][1] = position.y;
            result[3][2] = position.z;

            return result;
        }
    };

}
