//
// Created by Jeppe Nielsen on 22/01/2024.
//

#include "BgfxRenderer.hpp"
#include "Math.hpp"
#include <iostream>
#include <bx/math.h>

using namespace LittleCore;

BGFXRenderer::BGFXRenderer() {
    vertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)),
                                            LittleCore::Vertex::CreateVertexLayout());

    indexBuffer = bgfx::createDynamicIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(indices, sizeof(indices))
    );
}

void BGFXRenderer::BeginRender(bgfx::ViewId viewId, glm::mat4x4 view, glm::mat4x4 projection) {

    numMeshes = 0;
    numBatches = 0;
    bgfx::setViewTransform(viewId, &view[0][0], &projection[0][0]);
}
void BGFXRenderer::EndRender(bgfx::ViewId viewId) {



}

void BGFXRenderer::BeginBatch(bgfx::ViewId viewId) {

    currentVertex = 0;
    currentTriangle = 0;

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

    numMeshes++;

}
void BGFXRenderer::EndBatch(bgfx::ViewId viewId, bgfx::ProgramHandle shaderProgram) {
    if (currentVertex == 0 || currentTriangle==0) {
        return;
    }

    uint64_t state = 0
                     | BGFX_STATE_WRITE_R
                     | BGFX_STATE_WRITE_G
                     | BGFX_STATE_WRITE_B
                     | BGFX_STATE_WRITE_A
                     | BGFX_STATE_WRITE_Z
                     | BGFX_STATE_DEPTH_TEST_LESS
                     | BGFX_STATE_CULL_CW
                     | BGFX_STATE_MSAA
    //| BGFX_STATE_BLEND_ALPHA
    ;


    bgfx::update(vertexBuffer,0,bgfx::makeRef(vertices, currentVertex * sizeof (Vertex)));
    bgfx::update(indexBuffer,0,bgfx::makeRef(indices, currentTriangle* sizeof (uint16_t)));

    bgfx::setVertexBuffer(0, vertexBuffer, 0, currentVertex);
    bgfx::setIndexBuffer(indexBuffer, 0, currentTriangle);

    // Set render states.
    bgfx::setState(state);

    bgfx::submit(viewId, shaderProgram);

    numBatches++;
}
