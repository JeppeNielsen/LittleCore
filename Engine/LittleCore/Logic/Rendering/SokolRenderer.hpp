//
// Created by Jeppe Nielsen on 22/01/2024.
//

#pragma once
#include "Renderer.hpp"
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <tuple>

namespace LittleCore {
    class SokolRenderer : public Renderer {
    public:
        SokolRenderer();
        ~SokolRenderer();
        virtual void BeginRender(uint16_t viewId, glm::mat4x4 view, glm::mat4x4 projection, const Camera& camera) override;
        virtual void EndRender(uint16_t viewId) override;

        virtual void BeginBatch(uint16_t viewId) override;
        virtual void RenderMesh(const Mesh& mesh, const glm::mat4x4& world) override;
        virtual void EndBatch(uint16_t viewId, const ShaderResource* shaderResource, BlendMode blendMode) override;
        virtual void SetUniforms(const LittleCore::RenderableUniforms& uniforms) override;

    private:
        struct BatchBuffers {
            sg_buffer vertex = {SG_INVALID_ID};
            sg_buffer index = {SG_INVALID_ID};
            std::size_t vertexCapacityBytes = 0;
            std::size_t indexCapacityBytes = 0;
        };

        bool EnsureBuffer(sg_buffer& buffer, std::size_t& capacityBytes, bool isIndexBuffer, std::size_t requiredBytes);
        bool EnsureDefaultWhiteTexture();
        sg_view GetOrCreateTextureView(sg_image image);
        sg_pipeline GetOrCreatePipeline(sg_shader shaderProgram, BlendMode blendMode);
        BatchBuffers* AcquireBatchBuffers(std::size_t requiredVertexBytes, std::size_t requiredIndexBytes);

        glm::mat4x4 viewProjection = glm::mat4x4(1.0f);
        std::vector<Vertex> batchedVertices;
        std::vector<std::uint32_t> batchedIndices;
        std::vector<BatchBuffers> reusableBatchBuffers;
        bool hasBatchBufferFrame = false;
        std::uint32_t batchBufferFrameIndex = 0;
        std::size_t nextBatchBufferIndex = 0;
        sg_image currentTexture = {SG_INVALID_ID};
        sg_image defaultWhiteTexture = {SG_INVALID_ID};
        sg_sampler defaultSampler = {SG_INVALID_ID};
        std::unordered_map<uint32_t, sg_view> textureViewCache;
        struct PipelineKey {
            uint32_t shaderId;
            BlendMode blendMode;
            bool operator==(const PipelineKey& other) const {
                return shaderId == other.shaderId && blendMode == other.blendMode;
            }
        };
        struct PipelineKeyHash {
            std::size_t operator()(const PipelineKey& k) const {
                return std::hash<uint32_t>()(k.shaderId) ^ (std::hash<int>()(static_cast<int>(k.blendMode)) << 16);
            }
        };
        std::unordered_map<PipelineKey, sg_pipeline, PipelineKeyHash> pipelineCache;
        const RenderableUniforms* currentUniforms = nullptr;
    };

}
