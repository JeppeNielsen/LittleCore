//
// Created by Jeppe Nielsen on 15/01/2024.
//

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "SokolDirect.hpp"

namespace LittleCore {
    struct Vertex {
        glm::vec3 position;
        uint32_t color;
        glm::vec2 uv;

        static sg_vertex_layout_state CreateVertexLayout() {
            sg_vertex_layout_state layout{};
            return layout;
        };
    };
}

