//
// Created by Jeppe Nielsen on 15/01/2024.
//

#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <bgfx/bgfx.h>

namespace LittleCore {
    struct Vertex {
        glm::vec3 position;
        uint32_t color;
        glm::vec2 uv;

        static bgfx::VertexLayout CreateVertexLayout() {
            bgfx::VertexLayout layout;
            layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
            return layout;
        };
    };
}


