//
// Created by Jeppe Nielsen on 21/01/2024.
//

#pragma once
#include "Mesh.hpp"
#include <glm/mat4x4.hpp>
#include "Camera.hpp"
#include "RenderingStats.hpp"
#include "BlendMode.hpp"
#include "RenderableUniforms.hpp"

namespace LittleCore {
    struct Renderer {

        virtual ~Renderer() {};
        virtual void BeginRender(bgfx::ViewId viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) = 0;
        virtual void EndRender(bgfx::ViewId viewId) = 0;

        virtual void BeginBatch(bgfx::ViewId viewId) = 0;
        virtual void RenderMesh(const Mesh& mesh, const glm::mat4x4& world) = 0;
        virtual void EndBatch(bgfx::ViewId viewId, bgfx::ProgramHandle shaderProgram, BlendMode blendMode) = 0;
        virtual void SetTexture(const std::string& id, bgfx::TextureHandle texture) = 0;
        virtual void SetUniforms(const LittleCore::RenderableUniforms& uniforms) = 0;

        vec2 screenSize;
        RenderingStats stats;
    };


}