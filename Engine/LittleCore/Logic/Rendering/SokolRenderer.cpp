//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "SokolRenderer.hpp"

using namespace LittleCore;

SokolRenderer::SokolRenderer() = default;
SokolRenderer::~SokolRenderer() = default;

void SokolRenderer::BeginRender(uint16_t, glm::mat4x4, glm::mat4x4, const Camera&) {
    stats = {};
}

void SokolRenderer::EndRender(uint16_t) {}

void SokolRenderer::BeginBatch(uint16_t) {}

void SokolRenderer::RenderMesh(const Mesh& mesh, const glm::mat4x4&) {
    stats.numEntities++;
    stats.numVertices += mesh.vertices.size();
    stats.numTriangles += mesh.triangles.size();
}

void SokolRenderer::EndBatch(uint16_t, sg_shader, BlendMode) {
    stats.numRenderCalls++;
}

void SokolRenderer::SetUniforms(const RenderableUniforms&) {}

