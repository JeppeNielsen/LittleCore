//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "SokolRenderer.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>
#include <sokol_app.h>

namespace {
    std::size_t CalculateBufferCapacity(std::size_t requiredBytes) {
        constexpr std::size_t minimumCapacity = 1024;
        std::size_t capacity = minimumCapacity;
        while (capacity < requiredBytes) {
            capacity *= 2;
        }
        return capacity;
    }

    uint32_t GetPackedUniformElementSize(const sg_uniform_type type) {
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
                return sizeof(float);
            case SG_UNIFORMTYPE_FLOAT2:
                return sizeof(float) * 2;
            case SG_UNIFORMTYPE_FLOAT3:
                return sizeof(float) * 3;
            case SG_UNIFORMTYPE_FLOAT4:
                return sizeof(float) * 4;
            case SG_UNIFORMTYPE_INT:
                return sizeof(int);
            case SG_UNIFORMTYPE_INT2:
                return sizeof(int) * 2;
            case SG_UNIFORMTYPE_INT3:
                return sizeof(int) * 3;
            case SG_UNIFORMTYPE_INT4:
                return sizeof(int) * 4;
            case SG_UNIFORMTYPE_MAT4:
                return sizeof(glm::mat4x4);
            default:
                return 0;
        }
    }

    bool IsFloatVectorUniform(const sg_uniform_type type) {
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
            case SG_UNIFORMTYPE_FLOAT2:
            case SG_UNIFORMTYPE_FLOAT3:
            case SG_UNIFORMTYPE_FLOAT4:
                return true;
            default:
                return false;
        }
    }

    bool IsIntVectorUniform(const sg_uniform_type type) {
        switch (type) {
            case SG_UNIFORMTYPE_INT:
            case SG_UNIFORMTYPE_INT2:
            case SG_UNIFORMTYPE_INT3:
            case SG_UNIFORMTYPE_INT4:
                return true;
            default:
                return false;
        }
    }

    uint32_t GetUniformComponentCount(const sg_uniform_type type) {
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
            case SG_UNIFORMTYPE_INT:
                return 1;
            case SG_UNIFORMTYPE_FLOAT2:
            case SG_UNIFORMTYPE_INT2:
                return 2;
            case SG_UNIFORMTYPE_FLOAT3:
            case SG_UNIFORMTYPE_INT3:
                return 3;
            case SG_UNIFORMTYPE_FLOAT4:
            case SG_UNIFORMTYPE_INT4:
                return 4;
            default:
                return 0;
        }
    }

    void ApplyBlendMode(sg_pipeline_desc& pipelineDesc, LittleCore::BlendMode blendMode) {
        auto& blend = pipelineDesc.colors[0].blend;
        switch (blendMode) {
            case LittleCore::BlendMode::Off:
                blend.enabled = false;
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
        const sg_environment env = sg_query_desc().environment;
        pipelineDesc.sample_count = env.defaults.sample_count;
        pipelineDesc.depth.pixel_format = env.defaults.depth_format;
        pipelineDesc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
        pipelineDesc.depth.write_enabled = blendMode == LittleCore::BlendMode::Off;
        pipelineDesc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
        pipelineDesc.colors[0].write_mask = SG_COLORMASK_RGB; // don't write alpha
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

    uint32_t AlignTo(const uint32_t value, const uint32_t alignment) {
        if (alignment == 0) {
            return value;
        }
        const uint32_t remainder = value % alignment;
        return remainder == 0 ? value : (value + alignment - remainder);
    }

    uint32_t GetUniformAlignment(const sg_uniform_type type, const uint16_t arrayCount, const sg_uniform_layout layout) {
        if (layout == SG_UNIFORMLAYOUT_NATIVE) {
            return 1;
        }
        if (arrayCount > 1) {
            return 16;
        }
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
            case SG_UNIFORMTYPE_INT:
                return 4;
            case SG_UNIFORMTYPE_FLOAT2:
            case SG_UNIFORMTYPE_INT2:
                return 8;
            case SG_UNIFORMTYPE_FLOAT3:
            case SG_UNIFORMTYPE_FLOAT4:
            case SG_UNIFORMTYPE_INT3:
            case SG_UNIFORMTYPE_INT4:
            case SG_UNIFORMTYPE_MAT4:
                return 16;
            default:
                return 1;
        }
    }

    uint32_t GetUniformSize(const sg_uniform_type type, const uint16_t arrayCount, const sg_uniform_layout layout) {
        if (arrayCount <= 1) {
            switch (type) {
                case SG_UNIFORMTYPE_FLOAT:
                case SG_UNIFORMTYPE_INT:
                    return 4;
                case SG_UNIFORMTYPE_FLOAT2:
                case SG_UNIFORMTYPE_INT2:
                    return 8;
                case SG_UNIFORMTYPE_FLOAT3:
                case SG_UNIFORMTYPE_INT3:
                    return 12;
                case SG_UNIFORMTYPE_FLOAT4:
                case SG_UNIFORMTYPE_INT4:
                    return 16;
                case SG_UNIFORMTYPE_MAT4:
                    return 64;
                default:
                    return 0;
            }
        }

        if (layout == SG_UNIFORMLAYOUT_NATIVE) {
            const uint32_t elementSize = GetUniformSize(type, 1, layout);
            return elementSize * arrayCount;
        }

        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
            case SG_UNIFORMTYPE_FLOAT2:
            case SG_UNIFORMTYPE_FLOAT3:
            case SG_UNIFORMTYPE_FLOAT4:
            case SG_UNIFORMTYPE_INT:
            case SG_UNIFORMTYPE_INT2:
            case SG_UNIFORMTYPE_INT3:
            case SG_UNIFORMTYPE_INT4:
                return 16 * arrayCount;
            case SG_UNIFORMTYPE_MAT4:
                return 64 * arrayCount;
            default:
                return 0;
        }
    }

    const LittleCore::RenderableUniforms::UniformEntry* FindUniform(const LittleCore::RenderableUniforms* uniforms,
                                                                    const std::string_view id) {
        if (uniforms == nullptr) {
            return nullptr;
        }
        for (const auto& uniform : uniforms->GetUniforms()) {
            if (uniform.id == id) {
                return &uniform;
            }
        }
        return nullptr;
    }

    bool CopyUniformValue(const LittleCore::RenderableUniforms::UniformEntry& uniform,
                          const sg_glsl_shader_uniform& uniformDesc,
                          const sg_uniform_layout layout,
                          uint8_t* destination) {
        if (uniform.kind != LittleCore::RenderableUniforms::UniformEntry::Kind::Value) {
            return false;
        }

        const uint16_t arrayCount = uniformDesc.array_count > 0 ? uniformDesc.array_count : 1;
        if (uniform.type == uniformDesc.type && uniform.arrayCount == arrayCount) {
            const uint32_t elementSize = GetPackedUniformElementSize(uniform.type);
            if (elementSize == 0) {
                return false;
            }

            const std::size_t requiredBytes = static_cast<std::size_t>(elementSize) * uniform.arrayCount;
            if (uniform.data.size() != requiredBytes) {
                return false;
            }

            if (uniform.arrayCount <= 1 || layout == SG_UNIFORMLAYOUT_NATIVE) {
                std::memcpy(destination, uniform.data.data(), uniform.data.size());
                return true;
            }

            const uint32_t arrayStride = uniform.type == SG_UNIFORMTYPE_MAT4 ? 64 : 16;
            for (uint16_t arrayIndex = 0; arrayIndex < uniform.arrayCount; ++arrayIndex) {
                std::memcpy(destination + (arrayStride * arrayIndex),
                            uniform.data.data() + (elementSize * arrayIndex),
                            elementSize);
            }
            return true;
        }

        if (arrayCount != 1 || uniform.arrayCount != 1) {
            return false;
        }

        if (IsFloatVectorUniform(uniformDesc.type) && IsFloatVectorUniform(uniform.type)) {
            const uint32_t sourceComponentCount = GetUniformComponentCount(uniform.type);
            const uint32_t destinationComponentCount = GetUniformComponentCount(uniformDesc.type);
            if (sourceComponentCount >= destinationComponentCount &&
                uniform.data.size() >= static_cast<std::size_t>(sourceComponentCount * sizeof(float))) {
                std::memcpy(destination, uniform.data.data(), destinationComponentCount * sizeof(float));
                return true;
            }
        }

        if (IsIntVectorUniform(uniformDesc.type) && IsIntVectorUniform(uniform.type)) {
            const uint32_t sourceComponentCount = GetUniformComponentCount(uniform.type);
            const uint32_t destinationComponentCount = GetUniformComponentCount(uniformDesc.type);
            if (sourceComponentCount >= destinationComponentCount &&
                uniform.data.size() >= static_cast<std::size_t>(sourceComponentCount * sizeof(int))) {
                std::memcpy(destination, uniform.data.data(), destinationComponentCount * sizeof(int));
                return true;
            }
        }

        return false;
    }

    void WriteUniformData(const sg_glsl_shader_uniform& uniformDesc,
                          const sg_uniform_layout layout,
                          const LittleCore::RenderableUniforms* uniforms,
                          const glm::mat4x4& viewProjection,
                          uint8_t* destination) {
        if (destination == nullptr || uniformDesc.glsl_name == nullptr || uniformDesc.type == SG_UNIFORMTYPE_INVALID) {
            return;
        }

        const std::string_view uniformName = uniformDesc.glsl_name;
        if (uniformName == "u_modelViewProj" && uniformDesc.type == SG_UNIFORMTYPE_MAT4 && uniformDesc.array_count == 1) {
            std::memcpy(destination, &viewProjection, sizeof(viewProjection));
            return;
        }

        const auto* uniform = FindUniform(uniforms, uniformName);
        if (uniform == nullptr) {
            return;
        }

        CopyUniformValue(*uniform, uniformDesc, layout, destination);
    }

    std::vector<uint8_t> BuildUniformBlockData(const LittleCore::ShaderUniformBlockInfo& uniformBlock,
                                               const glm::mat4x4& viewProjection,
                                               const LittleCore::RenderableUniforms* uniforms) {
        std::vector<uint8_t> data(uniformBlock.size, 0);
        uint32_t offset = 0;
        for (const auto& uniformInfo : uniformBlock.uniforms) {
            const sg_glsl_shader_uniform uniformDesc{
                .type = uniformInfo.type,
                .array_count = uniformInfo.arrayCount,
                .glsl_name = uniformInfo.name.c_str(),
            };

            offset = AlignTo(offset, GetUniformAlignment(uniformDesc.type, uniformDesc.array_count, uniformBlock.layout));
            const uint32_t uniformSize = GetUniformSize(uniformDesc.type, uniformDesc.array_count, uniformBlock.layout);
            if (uniformSize == 0 || offset + uniformSize > data.size()) {
                break;
            }

            WriteUniformData(uniformDesc, uniformBlock.layout, uniforms, viewProjection, data.data() + offset);
            offset += uniformSize;
        }
        return data;
    }
}

using namespace LittleCore;

SokolRenderer::SokolRenderer() {
    batchedVertices.reserve(4096);
    batchedIndices.reserve(8192);
}

SokolRenderer::~SokolRenderer() {
    for (auto& buffers : reusableBatchBuffers) {
        lc_sg_destroy(buffers.vertex);
        lc_sg_destroy(buffers.index);
    }

    lc_sg_destroy(defaultWhiteTexture);

    if (defaultSampler.id != SG_INVALID_ID) {
        sg_destroy_sampler(defaultSampler);
        defaultSampler = {SG_INVALID_ID};
    }
}

void SokolRenderer::BeginRender(uint16_t, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) {
    stats = {};
    viewProjection = projection * view;

    const uint32_t frameIndex = sg_query_stats().cur_frame.frame_index;
    if (!hasBatchBufferFrame || batchBufferFrameIndex != frameIndex) {
        hasBatchBufferFrame = true;
        batchBufferFrameIndex = frameIndex;
        nextBatchBufferIndex = 0;
    }

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
    currentUniforms = nullptr;
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
        transformed.color = vertex.color;
        transformed.uv = vertex.uv;
        batchedVertices.push_back(transformed);
    }

    for (const auto index : mesh.triangles) {
        batchedIndices.push_back(vertexOffset + static_cast<uint32_t>(index));
    }
}

void SokolRenderer::EndBatch(uint16_t, const ShaderResource* shaderResource, BlendMode blendMode) {
    const sg_shader shaderProgram = shaderResource != nullptr ? shaderResource->handle : sg_shader{SG_INVALID_ID};
    if (shaderProgram.id == SG_INVALID_ID || batchedVertices.empty() || batchedIndices.empty()) {
        currentUniforms = nullptr;
        return;
    }
    const sg_resource_state shaderState = sg_query_shader_state(shaderProgram);
    if (shaderState == SG_RESOURCESTATE_FAILED || shaderState == SG_RESOURCESTATE_INVALID) {
        batchedVertices.clear();
        batchedIndices.clear();
        currentTexture = {SG_INVALID_ID};
        currentUniforms = nullptr;
        return;
    }

    sg_pipeline pipeline = CreatePipeline(shaderProgram, blendMode);
    if (pipeline.id == SG_INVALID_ID) {
        currentUniforms = nullptr;
        return;
    }

    const std::size_t vertexBufferSize = batchedVertices.size() * sizeof(Vertex);
    const std::size_t indexBufferSize = batchedIndices.size() * sizeof(std::uint32_t);
    BatchBuffers* batchBuffers = AcquireBatchBuffers(vertexBufferSize, indexBufferSize);
    if (batchBuffers == nullptr) {
        sg_destroy_pipeline(pipeline);
        currentUniforms = nullptr;
        return;
    }

    const sg_range vertexRange{batchedVertices.data(), vertexBufferSize};
    const sg_range indexRange{batchedIndices.data(), indexBufferSize};
    sg_update_buffer(batchBuffers->vertex, vertexRange);
    sg_update_buffer(batchBuffers->index, indexRange);

    const sg_shader_desc shaderDesc = sg_query_shader_desc(shaderProgram);
    const bool shaderUsesTexture =
        shaderDesc.views[0].texture.stage != SG_SHADERSTAGE_NONE &&
        shaderDesc.samplers[0].stage != SG_SHADERSTAGE_NONE;

    if (shaderUsesTexture && defaultSampler.id == SG_INVALID_ID) {
        sg_sampler_desc samplerDesc{};
        samplerDesc.min_filter = SG_FILTER_LINEAR;
        samplerDesc.mag_filter = SG_FILTER_LINEAR;
        samplerDesc.mipmap_filter = SG_FILTER_LINEAR;
        defaultSampler = sg_make_sampler(samplerDesc);
    }

    sg_image textureToBind = currentTexture;
    if (shaderUsesTexture && textureToBind.id == SG_INVALID_ID) {
        if (!EnsureDefaultWhiteTexture()) {
            sg_destroy_pipeline(pipeline);
            currentUniforms = nullptr;
            return;
        }
        textureToBind = defaultWhiteTexture;
    }

    sg_view textureView = {SG_INVALID_ID};
    if (shaderUsesTexture && textureToBind.id != SG_INVALID_ID && defaultSampler.id != SG_INVALID_ID) {
        sg_view_desc viewDesc{};
        viewDesc.texture.image = textureToBind;
        textureView = sg_make_view(viewDesc);
    }

    sg_bindings bindings = {};
    bindings.vertex_buffers[0] = batchBuffers->vertex;
    bindings.index_buffer = batchBuffers->index;
    if (textureView.id != SG_INVALID_ID) {
        bindings.views[0] = textureView;
        bindings.samplers[0] = defaultSampler;
    }

    sg_apply_pipeline(pipeline);
    sg_apply_bindings(bindings);

    for (const auto& uniformBlock : shaderResource->uniformBlocks) {
        std::vector<uint8_t> uniformData = BuildUniformBlockData(uniformBlock, viewProjection, currentUniforms);
        if (uniformData.empty()) {
            continue;
        }

        sg_range uniformRange{uniformData.data(), uniformData.size()};
        sg_apply_uniforms(uniformBlock.slot, uniformRange);
    }

    sg_draw(0, static_cast<int>(batchedIndices.size()), 1);
    stats.numRenderCalls++;

    if (textureView.id != SG_INVALID_ID) {
        sg_destroy_view(textureView);
    }
    sg_destroy_pipeline(pipeline);

    batchedVertices.clear();
    batchedIndices.clear();
    currentTexture = {SG_INVALID_ID};
    currentUniforms = nullptr;
}

bool SokolRenderer::EnsureBuffer(sg_buffer& buffer, std::size_t& capacityBytes, bool isIndexBuffer, std::size_t requiredBytes) {
    if (lc_sg_valid(buffer) && capacityBytes >= requiredBytes) {
        return true;
    }

    lc_sg_destroy(buffer);
    capacityBytes = 0;

    sg_buffer_desc bufferDesc{};
    bufferDesc.usage.vertex_buffer = !isIndexBuffer;
    bufferDesc.usage.index_buffer = isIndexBuffer;
    bufferDesc.usage.immutable = false;
    bufferDesc.usage.stream_update = true;
    bufferDesc.size = CalculateBufferCapacity(requiredBytes);
    buffer = sg_make_buffer(bufferDesc);
    if (!lc_sg_valid(buffer)) {
        return false;
    }

    capacityBytes = bufferDesc.size;
    return true;
}

bool SokolRenderer::EnsureDefaultWhiteTexture() {
    if (lc_sg_valid(defaultWhiteTexture)) {
        return true;
    }

    constexpr std::uint8_t whitePixel[] = {255, 255, 255, 255};

    sg_image_desc imageDesc{};
    imageDesc.width = 1;
    imageDesc.height = 1;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    imageDesc.data.mip_levels[0] = {whitePixel, sizeof(whitePixel)};
    defaultWhiteTexture = sg_make_image(imageDesc);
    return lc_sg_valid(defaultWhiteTexture);
}

SokolRenderer::BatchBuffers* SokolRenderer::AcquireBatchBuffers(std::size_t requiredVertexBytes, std::size_t requiredIndexBytes) {
    if (nextBatchBufferIndex >= reusableBatchBuffers.size()) {
        reusableBatchBuffers.emplace_back();
    }

    BatchBuffers& buffers = reusableBatchBuffers[nextBatchBufferIndex];
    nextBatchBufferIndex++;

    if (!EnsureBuffer(buffers.vertex, buffers.vertexCapacityBytes, false, requiredVertexBytes)) {
        return nullptr;
    }

    if (!EnsureBuffer(buffers.index, buffers.indexCapacityBytes, true, requiredIndexBytes)) {
        return nullptr;
    }

    return &buffers;
}

void SokolRenderer::SetUniforms(const RenderableUniforms& uniforms) {
    currentUniforms = &uniforms;
    currentTexture = {SG_INVALID_ID};
    sg_image fallbackTexture = {SG_INVALID_ID};

    for (const auto& uniform : uniforms.GetUniforms()) {
        if (uniform.kind != RenderableUniforms::UniformEntry::Kind::Texture) {
            continue;
        }

        if (uniform.id == "colorTexture") {
            currentTexture = uniform.texture;
            continue;
        }

        if (fallbackTexture.id == SG_INVALID_ID) {
            fallbackTexture = uniform.texture;
        }
    }

    if (currentTexture.id == SG_INVALID_ID) {
        currentTexture = fallbackTexture;
    }
}
