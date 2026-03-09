//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "SokolRenderer.hpp"
#include <algorithm>
#include <cstddef>
#include <cstring>

namespace {
    void ApplyBlendMode(sg_pipeline_desc& pipelineDesc, LittleCore::BlendMode blendMode) {
        auto& blend = pipelineDesc.colors[0].blend;
        switch (blendMode) {
            case LittleCore::BlendMode::Off:
                break;
            case LittleCore::BlendMode::Alpha:
                blend.enabled = true;
                blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
                blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
                blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                break;
            case LittleCore::BlendMode::Add:
                blend.enabled = true;
                blend.src_factor_rgb = SG_BLENDFACTOR_ONE;
                blend.dst_factor_rgb = SG_BLENDFACTOR_ONE;
                blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
                blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
                break;
            case LittleCore::BlendMode::Multiply:
                blend.enabled = true;
                blend.src_factor_rgb = SG_BLENDFACTOR_DST_COLOR;
                blend.dst_factor_rgb = SG_BLENDFACTOR_ZERO;
                blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
                blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                break;
        }
    }

    sg_pipeline CreatePipeline(sg_shader shaderProgram, LittleCore::BlendMode blendMode) {
        sg_pipeline_desc pipelineDesc = {};
        pipelineDesc.shader = shaderProgram;
        pipelineDesc.index_type = SG_INDEXTYPE_UINT32;
        pipelineDesc.sample_count = 1;
        pipelineDesc.depth.pixel_format = SG_PIXELFORMAT_NONE;
        pipelineDesc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
        pipelineDesc.layout.buffers[0].stride = sizeof(LittleCore::Vertex);
        pipelineDesc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
        pipelineDesc.layout.attrs[0].offset = offsetof(LittleCore::Vertex, position);
        pipelineDesc.layout.attrs[1].format = SG_VERTEXFORMAT_UBYTE4N;
        pipelineDesc.layout.attrs[1].offset = offsetof(LittleCore::Vertex, color);
        pipelineDesc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2;
        pipelineDesc.layout.attrs[2].offset = offsetof(LittleCore::Vertex, uv);
        ApplyBlendMode(pipelineDesc, blendMode);
        return sg_make_pipeline(pipelineDesc);
    }
}

using namespace LittleCore;

SokolRenderer::SokolRenderer() {
    batchedVertices.reserve(4096);
    batchedIndices.reserve(8192);
}

SokolRenderer::~SokolRenderer() {
    if (defaultSampler.id != SG_INVALID_ID) {
        sg_destroy_sampler(defaultSampler);
        defaultSampler = {SG_INVALID_ID};
    }
}

void SokolRenderer::BeginRender(uint16_t, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) {
    stats = {};
    viewProjection = projection * view;

    const int renderWidth = std::max(0, static_cast<int>(screenSize.x));
    const int renderHeight = std::max(0, static_cast<int>(screenSize.y));
    if (renderWidth <= 0 || renderHeight <= 0) {
        return;
    }

    const float minX = std::clamp(camera.viewRect.min.x, 0.0f, 1.0f);
    const float minY = std::clamp(camera.viewRect.min.y, 0.0f, 1.0f);
    const float maxX = std::clamp(camera.viewRect.max.x, 0.0f, 1.0f);
    const float maxY = std::clamp(camera.viewRect.max.y, 0.0f, 1.0f);

    const int viewportX = static_cast<int>(minX * screenSize.x);
    const int viewportY = static_cast<int>(minY * screenSize.y);
    const int viewportW = std::max(0, static_cast<int>((maxX - minX) * screenSize.x));
    const int viewportH = std::max(0, static_cast<int>((maxY - minY) * screenSize.y));

    if (viewportW > 0 && viewportH > 0) {
        const bool originTopLeft = sg_query_features().origin_top_left;
        sg_apply_viewport(viewportX, viewportY, viewportW, viewportH, originTopLeft);
        sg_apply_scissor_rect(viewportX, viewportY, viewportW, viewportH, originTopLeft);
    }
}

void SokolRenderer::EndRender(uint16_t) {}

void SokolRenderer::BeginBatch(uint16_t) {
    batchedVertices.clear();
    batchedIndices.clear();
    currentTexture = {SG_INVALID_ID};
}

void SokolRenderer::RenderMesh(const Mesh& mesh, const glm::mat4x4& world) {
    stats.numEntities++;
    stats.numVertices += static_cast<int>(mesh.vertices.size());
    stats.numTriangles += static_cast<int>(mesh.triangles.size());

    if (mesh.vertices.empty() || mesh.triangles.empty()) {
        return;
    }

    const uint32_t vertexOffset = static_cast<uint32_t>(batchedVertices.size());
    batchedVertices.reserve(batchedVertices.size() + mesh.vertices.size());
    batchedIndices.reserve(batchedIndices.size() + mesh.triangles.size());

    for (const auto& vertex : mesh.vertices) {
        Vertex transformed = vertex;
        const vec4 worldPosition = world * vec4(vertex.position, 1.0f);
        transformed.position = vec3(worldPosition);
        batchedVertices.push_back(transformed);
    }

    for (const auto index : mesh.triangles) {
        batchedIndices.push_back(vertexOffset + static_cast<uint32_t>(index));
    }
}

void SokolRenderer::EndBatch(uint16_t, sg_shader shaderProgram, BlendMode blendMode) {
    if (shaderProgram.id == SG_INVALID_ID || batchedVertices.empty() || batchedIndices.empty()) {
        return;
    }
    const sg_resource_state shaderState = sg_query_shader_state(shaderProgram);
    if (shaderState == SG_RESOURCESTATE_FAILED || shaderState == SG_RESOURCESTATE_INVALID) {
        batchedVertices.clear();
        batchedIndices.clear();
        currentTexture = {SG_INVALID_ID};
        return;
    }

    sg_pipeline pipeline = CreatePipeline(shaderProgram, blendMode);
    if (pipeline.id == SG_INVALID_ID) {
        return;
    }

    sg_buffer_desc vertexBufferDesc = {};
    vertexBufferDesc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    vertexBufferDesc.data.ptr = batchedVertices.data();
    vertexBufferDesc.data.size = batchedVertices.size() * sizeof(Vertex);
    sg_buffer vertexBuffer = sg_make_buffer(vertexBufferDesc);

    sg_buffer_desc indexBufferDesc = {};
    indexBufferDesc.type = SG_BUFFERTYPE_INDEXBUFFER;
    indexBufferDesc.data.ptr = batchedIndices.data();
    indexBufferDesc.data.size = batchedIndices.size() * sizeof(uint32_t);
    sg_buffer indexBuffer = sg_make_buffer(indexBufferDesc);

    if (vertexBuffer.id == SG_INVALID_ID || indexBuffer.id == SG_INVALID_ID) {
        if (vertexBuffer.id != SG_INVALID_ID) {
            sg_destroy_buffer(vertexBuffer);
        }
        if (indexBuffer.id != SG_INVALID_ID) {
            sg_destroy_buffer(indexBuffer);
        }
        sg_destroy_pipeline(pipeline);
        return;
    }

    if (currentTexture.id != SG_INVALID_ID && defaultSampler.id == SG_INVALID_ID) {
        sg_sampler_desc samplerDesc = {};
        samplerDesc.min_filter = SG_FILTER_LINEAR;
        samplerDesc.mag_filter = SG_FILTER_LINEAR;
        samplerDesc.mipmap_filter = SG_FILTER_LINEAR;
        defaultSampler = sg_make_sampler(samplerDesc);
    }

    sg_bindings bindings = {};
    bindings.vertex_buffers[0] = vertexBuffer;
    bindings.index_buffer = indexBuffer;
    if (currentTexture.id != SG_INVALID_ID && defaultSampler.id != SG_INVALID_ID) {
        bindings.images[0] = currentTexture;
        bindings.samplers[0] = defaultSampler;
    }

    sg_apply_pipeline(pipeline);
    sg_apply_bindings(bindings);

    const sg_shader_desc shaderDesc = sg_query_shader_desc(shaderProgram);
    if (shaderDesc.uniform_blocks[0].stage != SG_SHADERSTAGE_NONE && shaderDesc.uniform_blocks[0].size > 0) {
        const uint32_t uniformSize = shaderDesc.uniform_blocks[0].size;
        if (uniformSize <= sizeof(glm::mat4x4)) {
            sg_range uniformRange{&viewProjection, uniformSize};
            sg_apply_uniforms(0, uniformRange);
        } else {
            std::vector<uint8_t> uniformData(uniformSize, 0);
            std::memcpy(uniformData.data(), &viewProjection, sizeof(glm::mat4x4));
            sg_range uniformRange{uniformData.data(), uniformData.size()};
            sg_apply_uniforms(0, uniformRange);
        }
    }

    sg_draw(0, static_cast<int>(batchedIndices.size()), 1);
    stats.numRenderCalls++;

    sg_destroy_buffer(vertexBuffer);
    sg_destroy_buffer(indexBuffer);
    sg_destroy_pipeline(pipeline);

    batchedVertices.clear();
    batchedIndices.clear();
    currentTexture = {SG_INVALID_ID};
}

void SokolRenderer::SetUniforms(const RenderableUniforms& uniforms) {
    currentTexture = {SG_INVALID_ID};
    sg_image fallbackTexture = {SG_INVALID_ID};

    for (const auto& uniform : uniforms.GetUniforms()) {
        if (uniform.kind != RenderableUniforms::UniformEntry::Kind::Texture) {
            continue;
        }

        if (uniform.id == "colorTexture") {
            currentTexture = uniform.value.tex;
            return;
        }

        if (fallbackTexture.id == SG_INVALID_ID) {
            fallbackTexture = uniform.value.tex;
        }
    }

    currentTexture = fallbackTexture;
}
