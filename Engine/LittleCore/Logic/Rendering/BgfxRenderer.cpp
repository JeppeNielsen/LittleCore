//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "BgfxRenderer.hpp"
#include "Math.hpp"
#include <bx/math.h>
#include "Math.hpp"

using namespace LittleCore;

BGFXRenderer::BGFXRenderer() {
    vertexLayout = LittleCore::Vertex::CreateVertexLayout();
}

BGFXRenderer::~BGFXRenderer() {

}

void BGFXRenderer::BeginRender(bgfx::ViewId viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) {

    stats = {};
    bgfx::setViewTransform(viewId, &view[0][0], &projection[0][0]);

    const auto& viewRect = camera.viewRect;

    uint16_t x = (uint16_t) screenSize.x * viewRect.min.x;
    uint16_t y = (uint16_t) screenSize.y * viewRect.min.y;

    uint16_t width = (uint16_t) screenSize.x * (viewRect.max.x - viewRect.min.x);
    uint16_t height = (uint16_t) screenSize.y * (viewRect.max.y - viewRect.min.y);

    bgfx::setViewRect(viewId, x, y, width, height);
    if (camera.clearBackground) {
        bgfx::setViewClear(viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, camera.clearColor, 1.0f, 0);
    }


    currentVertex = 0;
    currentTriangle = 0;
    bgfx::allocTransientBuffers(&vertexBuffer, vertexLayout, maxVertices, &indexBuffer, maxIndices);
    vertices = (Vertex*)vertexBuffer.data;
    indices = (uint16*)indexBuffer.data;
}

void BGFXRenderer::EndRender(bgfx::ViewId viewId) {

}

void BGFXRenderer::BeginBatch(bgfx::ViewId viewId) {
    startBatchVertex = currentVertex;
    startBatchIndex = currentTriangle;
}

void BGFXRenderer::RenderMesh(const Mesh& mesh, const glm::mat4x4& world) {

    int baseTriangleIndex = currentVertex;
    for (int i = 0; i < mesh.vertices.size(); ++i) {
        const auto &source = mesh.vertices[i];
        auto &dest = vertices[currentVertex];

        const glm::vec4 pos4d = vec4(source.position, 1.0f);
        dest.position = world * pos4d;

        dest.color = source.color;
        dest.uv = source.uv;

        ++currentVertex;
    }

    for (int i = 0; i < mesh.triangles.size(); ++i) {
        indices[currentTriangle] = baseTriangleIndex + mesh.triangles[i];
        ++currentTriangle;
    }

    stats.numEntities++;
    stats.numVertices+=mesh.vertices.size();
    stats.numTriangles+=mesh.triangles.size();
}

void BGFXRenderer::EndBatch(bgfx::ViewId viewId, bgfx::ProgramHandle shaderProgram, BlendMode blendMode) {
    if (currentVertex == 0 || currentTriangle==0) {
        return;
    }

    uint64_t state = 0
                     | BGFX_STATE_WRITE_R
                     | BGFX_STATE_WRITE_G
                     | BGFX_STATE_WRITE_B
                     | BGFX_STATE_WRITE_A
                     | BGFX_STATE_DEPTH_TEST_LESS
                     | BGFX_STATE_CULL_CW
                     | BGFX_STATE_MSAA
    ;

    if (blendMode == BlendMode::Off) {
        state |= BGFX_STATE_WRITE_Z;
    } else if (blendMode == BlendMode::Alpha) {
        state |= BGFX_STATE_BLEND_ALPHA;
    }

    bgfx::setState(state);

    bgfx::setVertexBuffer(0, &vertexBuffer, 0, currentVertex);
    bgfx::setIndexBuffer(&indexBuffer, startBatchIndex, currentTriangle - startBatchIndex);

    bgfx::submit(viewId, shaderProgram);
    stats.numRenderCalls++;
}

void BGFXRenderer::SetTexture(const std::string& id, bgfx::TextureHandle texture) {
    if (currentVertex == 0 || currentTriangle==0) {
        return;
    }
    auto textureUniform = uniformCollection.GetHandle(id, bgfx::UniformType::Sampler);
    bgfx::setTexture(0, textureUniform, texture);
}

void BGFXRenderer::SetUniforms(const RenderableUniforms& uniforms) {

    for(const auto& e : uniforms.GetUniforms()) {
        switch (e.kind) {
            case RenderableUniforms::UniformEntry::Kind::Texture:{
                SetTexture(e.id, e.value.tex);
                break;
            }
            case RenderableUniforms::UniformEntry::Kind::Vec4: {
                auto uniform = uniformCollection.GetHandle(e.id, bgfx::UniformType::Vec4);
                bgfx::setUniform(uniform, glm::value_ptr(e.value.v4));
                break;
            }
            case RenderableUniforms::UniformEntry::Kind::Mat3x3: {
                auto uniform = uniformCollection.GetHandle(e.id, bgfx::UniformType::Mat3);
                bgfx::setUniform(uniform, glm::value_ptr(e.value.m3));
                break;

            }
            case RenderableUniforms::UniformEntry::Kind::Mat4x4: {
                auto uniform = uniformCollection.GetHandle(e.id, bgfx::UniformType::Mat4);
                bgfx::setUniform(uniform, glm::value_ptr(e.value.m4));
                break;
            }
        }
    }

}

