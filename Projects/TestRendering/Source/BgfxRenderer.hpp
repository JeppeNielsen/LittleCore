//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "Renderer.hpp"
#include "Camera.hpp"

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
        virtual void BeginRender(bgfx::ViewId viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) override;
        virtual void EndRender(bgfx::ViewId viewId) override;

        virtual void BeginBatch(bgfx::ViewId viewId) override;
        virtual void RenderMesh(const Mesh& mesh, const glm::mat4x4& world) override;
        virtual void EndBatch(bgfx::ViewId viewId, bgfx::ProgramHandle shaderProgram) override;

        int numMeshes;
        int numBatches;
    };

}