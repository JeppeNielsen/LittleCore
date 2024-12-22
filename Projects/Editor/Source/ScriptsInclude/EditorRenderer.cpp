//
// Created by Jeppe Nielsen on 16/12/2024.
//

#include "EditorRenderer.hpp"

EditorRenderer::EditorRenderer(EditorRenderer::RenderFunction renderFunction,
                               EditorRenderer::GetTextureFunction getTextureFunction) :
                               renderFunction(renderFunction),
                               getTextureFunction(getTextureFunction) {

}

void EditorRenderer::Render(const std::string &id, int width, int height, EditorRenderer::Callback callback) {
    renderFunction(id, width, height, callback);
}

ImTextureID EditorRenderer::GetTexture(const std::string &id) {
    return getTextureFunction(id);
}
