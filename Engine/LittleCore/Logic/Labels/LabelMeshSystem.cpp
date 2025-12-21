//
// Created by Jeppe Nielsen on 02/12/2025.
//

#include "LabelMeshSystem.hpp"
#include <iostream>
#include "Renderable.hpp"

using namespace LittleCore;

LabelMeshSystem::LabelMeshSystem(entt::registry& registry) :
    SystemBase(registry),
    observer(registry, entt::collector
            .update<Label>().where<Mesh, Renderable>()
            .update<Mesh>().where<Label, Renderable>()
            .group<Mesh, Label, Renderable>()) {
}


void WriteGlyph(Mesh& mesh, Label& label, uint32_t glyphId, float& offset) {

    float yPen = 0;

    FontAtlas::GlyphQuad quad;
    if (!label.font->atlas.getQuad(glyphId, offset, yPen, quad)) {
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

}

void LabelMeshSystem::Update() {

    for(auto entity : observer) {
        auto& label = registry.get<Label>(entity);

        if (!label.font) {
            continue;
        }

        auto& mesh = registry.get<Mesh>(entity);
        auto& renderable = registry.get<Renderable>(entity);

        mesh.handle.Clear();

        mesh.triangles.clear();
        mesh.vertices.clear();

        float offset = 0;
        for (int i = 0; i < label.text.size(); ++i) {
            WriteGlyph(mesh, label, label.text[i], offset);
        }

        renderable.uniforms.Set("colorTexture", label.font->atlas.pageTexture(0));
        renderable.uniforms.Set("outlineSize", vec4(label.outlineSize));
        renderable.uniforms.Set("outlineColor", label.outlineColor);

        registry.patch<Mesh>(entity);
    }
    observer.clear();

}

void LabelMeshSystem::Reload() {
    auto view = registry.view<const Label, Renderable>();

    for(auto [entity, label, renderable] : view.each()) {
        renderable.uniforms.Set("colorTexture", label.font->atlas.pageTexture(0));
        renderable.uniforms.Set("outlineSize", vec4(label.outlineSize));
        registry.patch<Label>(entity);
    }
}
