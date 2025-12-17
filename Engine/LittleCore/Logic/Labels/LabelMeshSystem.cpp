//
// Created by Jeppe Nielsen on 02/12/2025.
//

#include "LabelMeshSystem.hpp"
#include <iostream>

using namespace LittleCore;

LabelMeshSystem::LabelMeshSystem(entt::registry& registry) :
    SystemBase(registry),
    observer(registry, entt::collector
            .update<Label>().where<Mesh, Texturable>()
            .update<Mesh>().where<Label, Texturable>()
            .group<Mesh, Label, Texturable>()) {
}


void WriteGlyph(Mesh& mesh, Label& label, uint32_t glyphId, float& offset) {

    float yPen = 0;

    SdfFontAtlasDynamic::GlyphQuad quad;
    if (!label.font->atlasDynamic.getQuad(glyphId, offset, yPen, quad)) {
        return;
    }

    uint16_t index = mesh.vertices.size();

    mesh.triangles.push_back(index);
    mesh.triangles.push_back(index+1);
    mesh.triangles.push_back(index+2);

    mesh.triangles.push_back(index);
    mesh.triangles.push_back(index+2);
    mesh.triangles.push_back(index+3);


    mesh.vertices.push_back({
                                    {quad.x0, -quad.y0,0},
                                    0xFFFFFF,
                                    {quad.u0, quad.v0}
                            });

    mesh.vertices.push_back({
                                    {quad.x0, -quad.y1,0},
                                    0xFFFFFF,
                                    {quad.u0, quad.v1}
                            });

    mesh.vertices.push_back({
                                    {quad.x1, -quad.y1,0},
                                    0xFFFFFF,
                                    {quad.u1, quad.v1}
                            });

    mesh.vertices.push_back({
                                    {quad.x1, -quad.y0,0},
                                    0xFFFFFF,
                                    {quad.u1, quad.v0}
                            });
    return;

    auto& info = label.font->EnsureGlyph(glyphId);


    glm::vec3 position = {offset,0,0};

    float fontSize = 1.0f;

    glm::vec2 bottomLeft = {info.bounds.l * fontSize,info.bounds.b * fontSize};
    glm::vec2 topRight = {info.bounds.r * fontSize,info.bounds.t * fontSize};

    mesh.vertices.push_back({
                                    position + glm::vec3(bottomLeft.x, bottomLeft.y,0),
                                    0xFFFFFF,
                                    {info.u0, info.v0}
                            });

    mesh.vertices.push_back({
                                    position + glm::vec3(bottomLeft.x,topRight.y,0),
                                    0xFFFFFF,
                                    {info.u0, info.v1}
                            });

    mesh.vertices.push_back({
                                    position + glm::vec3(topRight.x,topRight.y,0),
                                    0xFFFFFF,
                                    {info.u1, info.v1}
                            });

    mesh.vertices.push_back({
                                    position + glm::vec3(topRight.x,bottomLeft.y,0),
                                    0xFFFFFF,
                                    {info.u1, info.v0}
                            });

    offset += info.advance;
}

void LabelMeshSystem::Update() {

    for(auto entity : observer) {
        auto& label = registry.get<Label>(entity);

        if (!label.font) {
            continue;
        }

        auto& mesh = registry.get<Mesh>(entity);
        auto& texturable = registry.get<Texturable>(entity);

        texturable.handle.Clear();
        mesh.handle.Clear();

        mesh.triangles.clear();
        mesh.vertices.clear();

        float offset = 0;
        for (int i = 0; i < label.text.size(); ++i) {
            WriteGlyph(mesh, label, label.text[i], offset);
        }

        texturable.texture = label.font->atlasDynamic.pageTexture(0);

        registry.patch<Mesh>(entity);
    }
    observer.clear();

}

void LabelMeshSystem::Reload() {
    auto view = registry.view<const Label, Texturable>();

    for(auto [entity, label, texturable] : view.each()) {
        texturable.texture = label.font->atlasDynamic.pageTexture(0);
        registry.patch<Label>(entity);
    }
}
