//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include "ResizableFrameBuffer.hpp"
#include <sokol_app.h>

using namespace LittleCore;

void ResizableFrameBuffer::Render(uint16_t width, uint16_t height, const std::function<void()>& renderFunction) {
    if (width == 0 || height == 0) {
        return;
    }
    EnsureResources(width, height);
    if (!lc_sg_valid(frameBuffer)) {
        return;
    }
    sg_pass_action passAction{};
    passAction.colors[0].load_action = SG_LOADACTION_CLEAR;
    passAction.colors[0].store_action = SG_STOREACTION_STORE;
    passAction.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 1.0f};
    passAction.depth.load_action = SG_LOADACTION_CLEAR;
    passAction.depth.store_action = SG_STOREACTION_DONTCARE;
    passAction.depth.clear_value = 1.0f;

    sg_pass pass{};
    pass.action = passAction;
    pass.attachments = frameBuffer;
    sg_begin_pass(pass);
    renderFunction();
    sg_end_pass();
}

void ResizableFrameBuffer::EnsureResources(uint16_t width, uint16_t height) {
    if (width == 0 || height == 0) {
        lc_sg_destroy(frameBuffer);
        lc_sg_destroy(texture);
        lc_sg_destroy(depthTexture);
        this->width = 0;
        this->height = 0;
        return;
    }

    if (this->width == width && this->height == height) {
        return;
    }

    this->width = width;
    this->height = height;

    lc_sg_destroy(frameBuffer);
    lc_sg_destroy(texture);
    lc_sg_destroy(depthTexture);

    sg_image_desc imageDesc{};
    imageDesc.render_target = true;
    imageDesc.width = this->width;
    imageDesc.height = this->height;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    texture = sg_make_image(imageDesc);

    sg_image_desc depthDesc{};
    depthDesc.render_target = true;
    depthDesc.width = this->width;
    depthDesc.height = this->height;
    depthDesc.pixel_format = static_cast<sg_pixel_format>(sapp_depth_format());
    depthTexture = sg_make_image(depthDesc);

    sg_attachments_desc attachmentsDesc{};
    attachmentsDesc.colors[0].image = texture;
    attachmentsDesc.depth_stencil.image = depthTexture;
    frameBuffer = sg_make_attachments(attachmentsDesc);
}

ResizableFrameBuffer::~ResizableFrameBuffer() {
    lc_sg_destroy(frameBuffer);
    lc_sg_destroy(texture);
    lc_sg_destroy(depthTexture);
}
