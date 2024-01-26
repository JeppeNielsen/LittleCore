//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "Renderer.hpp"

namespace LittleCore {
    class BGFXRenderer : public Renderer {
    private:
        static constexpr size_t maxVertices = 1 << 16;
        static constexpr size_t maxIndices = 1 << 16;

        bgfx::DynamicVertexBufferHandle vertexBuffer;
        bgfx::DynamicIndexBufferHandle indexBuffer;
        LittleCore::Vertex vertices[maxVertices];
        uint16_t indices[maxIndices];

        int currentVertex = 0;
        int currentTriangle = 0;


    public:
        BGFXRenderer();

        virtual void BeginRender() override;

        virtual void Render(const LittleCore::Mesh &mesh, const glm::mat4x4 &world) override;

        virtual void EndRender() override;

        int count = 0;

    };

}