//
// Created by Jeppe Nielsen on 16/12/2024.
//

#include "EditorRendererOld.hpp"
#include "FrameBufferFactory.hpp"
#include <iostream>

EditorRendererOld::EditorRendererOld(LittleCore::Renderer &renderer) : renderer(renderer){
    frameBufferFactory = new FrameBufferFactory();
}

EditorRendererOld::~EditorRendererOld() {
    delete frameBufferFactory;
}

void EditorRendererOld::Render(const std::string &id, int width, int height, EditorRendererOld::Callback callback) {
    auto& frameBuffer = frameBufferFactory->CreateBuffer(id, width, height);

    bgfx::setViewFrameBuffer(0, frameBuffer.framebuffer);
    
    bgfx::touch(0);

    callback(renderer);

    bgfx::frame();

    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);

}

ImTextureID EditorRendererOld::GetTexture(const std::string &id) {
    FrameBufferFactory::FrameBuffer* frameBuffer;
    if (!frameBufferFactory->TryGetFrameBuffer(id, &frameBuffer)) {
        return nullptr;
    }
    return (ImTextureID)(uintptr_t)frameBuffer->renderTexture.idx;
}