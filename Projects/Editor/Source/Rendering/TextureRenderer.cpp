//
// Created by Jeppe Nielsen on 16/12/2024.
//

#include "TextureRenderer.hpp"
#include "FrameBufferFactory.hpp"
#include <cstdint>
#include <iostream>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include <util/sokol_imgui.h>

TextureRenderer::TextureRenderer(LittleCore::Renderer &renderer) : renderer(renderer){
    frameBufferFactory = new FrameBufferFactory();
}

TextureRenderer::~TextureRenderer() {
    delete frameBufferFactory;
}

void TextureRenderer::Render(const std::string &id, int width, int height, TextureRenderer::Callback callback) {
    auto& frameBuffer = frameBufferFactory->CreateBuffer(id, width, height);

    renderer.screenSize = {width, height};

    sg_pass_action passAction{};
    passAction.colors[0].load_action = SG_LOADACTION_CLEAR;
    passAction.colors[0].store_action = SG_STOREACTION_STORE;
    passAction.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};

    sg_pass pass{};
    pass.action = passAction;
    pass.attachments = frameBuffer.framebuffer;
    sg_begin_pass(pass);

    callback(renderer);

    sg_end_pass();

}

ImTextureID TextureRenderer::GetTexture(const std::string &id) {
    FrameBufferFactory::FrameBuffer* frameBuffer;
    if (!frameBufferFactory->TryGetFrameBuffer(id, &frameBuffer)) {
        return 0;
    }
    if (!lc_sg_valid(frameBuffer->renderTexture)) {
        return 0;
    }
    return static_cast<ImTextureID>(simgui_imtextureid(frameBuffer->renderTexture));
}
