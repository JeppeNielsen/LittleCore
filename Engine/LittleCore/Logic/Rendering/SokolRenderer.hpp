//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "Renderer.hpp"
#include <vector>
#include <cstdint>

namespace LittleCore {
    class SokolRenderer : public Renderer {
    public:
        SokolRenderer();
        ~SokolRenderer();
        virtual void BeginRender(uint16_t viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) override;
        virtual void EndRender(uint16_t viewId) override;

        virtual void BeginBatch(uint16_t viewId) override;
        virtual void RenderMesh(const Mesh& mesh, const glm::mat4x4& world) override;
        virtual void EndBatch(uint16_t viewId, sg_shader shaderProgram, BlendMode blendMode) override;
        virtual void SetUniforms(const LittleCore::RenderableUniforms& uniforms) override;

    private:
        glm::mat4x4 viewProjection = glm::mat4x4(1.0f);
        std::vector<Vertex> batchedVertices;
        std::vector<std::uint32_t> batchedIndices;
        sg_image currentTexture = {SG_INVALID_ID};
        sg_sampler defaultSampler = {SG_INVALID_ID};
    };

}
