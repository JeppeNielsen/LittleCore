//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "Renderer.hpp"

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
    };

}
