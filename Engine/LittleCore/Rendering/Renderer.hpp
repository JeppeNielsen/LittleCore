//
// Created by Jeppe Nielsen on 21/01/2024.
//

#pragma once
#include "Mesh.hpp"
#include <glm/mat4x4.hpp>

namespace LittleCore {
    struct Renderer {

        virtual ~Renderer() {};
        virtual void Render(const Mesh& mesh, const glm::mat4x4& world) = 0;

    };


}