//
// Created by Jeppe Nielsen on 16/12/2024.
//
#include "FrameBufferFactory.hpp"

FrameBufferFactory::FrameBuffer& FrameBufferFactory::CreateBuffer(const std::string &id, int width, int height) {

    FrameBuffer& frameBuffer = frameBuffers[id];
    if (width <= 0 || height <= 0) {
        frameBuffer.width = 0;
        frameBuffer.height = 0;
        lc_sg_destroy(frameBuffer.framebuffer);
        lc_sg_destroy(frameBuffer.renderTexture);
        return frameBuffer;
    }

    if (frameBuffer.width == width && frameBuffer.height == height && lc_sg_valid(frameBuffer.framebuffer)) {
        return frameBuffer;
    }

    frameBuffer.width = width;
    frameBuffer.height = height;
    lc_sg_destroy(frameBuffer.framebuffer);
    lc_sg_destroy(frameBuffer.renderTexture);

    sg_image_desc imageDesc{};
    imageDesc.render_target = true;
    imageDesc.width = width;
    imageDesc.height = height;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    frameBuffer.renderTexture = sg_make_image(imageDesc);

    sg_attachments_desc attachmentsDesc{};
    attachmentsDesc.colors[0].image = frameBuffer.renderTexture;
    frameBuffer.framebuffer = sg_make_attachments(attachmentsDesc);

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
