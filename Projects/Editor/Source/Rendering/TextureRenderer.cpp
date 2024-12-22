//
// Created by Jeppe Nielsen on 16/12/2024.
//

#include "TextureRenderer.hpp"
#include "FrameBufferFactory.hpp"
#include <iostream>

TextureRenderer::TextureRenderer(LittleCore::Renderer &renderer) : renderer(renderer){
    frameBufferFactory = new FrameBufferFactory();
}

TextureRenderer::~TextureRenderer() {
    delete frameBufferFactory;
}

void TextureRenderer::Render(const std::string &id, int width, int height, TextureRenderer::Callback callback) {
    auto& frameBuffer = frameBufferFactory->CreateBuffer(id, width, height);

    bgfx::setViewFrameBuffer(0, frameBuffer.framebuffer);

    bgfx::touch(0);

    callback(renderer);

    bgfx::frame();

    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);

}

ImTextureID TextureRenderer::GetTexture(const std::string &id) {
    FrameBufferFactory::FrameBuffer* frameBuffer;
    if (!frameBufferFactory->TryGetFrameBuffer(id, &frameBuffer)) {
        return nullptr;
    }
    return (ImTextureID)(uintptr_t)frameBuffer->renderTexture.idx;
}