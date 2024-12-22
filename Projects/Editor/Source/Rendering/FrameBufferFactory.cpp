//
// Created by Jeppe Nielsen on 16/12/2024.
//
#include "FrameBufferFactory.hpp"

FrameBufferFactory::FrameBuffer& FrameBufferFactory::CreateBuffer(const std::string &id, int width, int height) {

    auto found = frameBuffers.find(id);

    if (found!=frameBuffers.end()) {
        return found->second;
    }

    FrameBuffer& frameBuffer = frameBuffers[id];

    frameBuffer.renderTexture = bgfx::createTexture2D(
            width, height,
            false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT
    );

    frameBuffer.framebuffer = bgfx::createFrameBuffer(1, &frameBuffer.renderTexture, true);

    return frameBuffer;
}

bool FrameBufferFactory::TryGetFrameBuffer(const std::string& id, FrameBufferFactory::FrameBuffer** frameBuffer) {
    auto found = frameBuffers.find(id);

    if (found==frameBuffers.end()) {
        *frameBuffer = nullptr;
        return false;
    }

    *frameBuffer = &found->second;
    return true;
}

