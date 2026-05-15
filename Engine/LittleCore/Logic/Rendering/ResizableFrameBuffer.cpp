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
    if (!lc_sg_valid(colorView)) {
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
    pass.attachments.colors[0] = colorView;
    pass.attachments.depth_stencil = depthView;
    sg_begin_pass(pass);
    renderFunction();
    sg_end_pass();
}

void ResizableFrameBuffer::EnsureResources(uint16_t width, uint16_t height) {
    if (width == 0 || height == 0) {
        lc_sg_destroy(textureView);
        lc_sg_destroy(colorView);
        lc_sg_destroy(depthView);
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

    lc_sg_destroy(textureView);
    lc_sg_destroy(colorView);
    lc_sg_destroy(depthView);
    lc_sg_destroy(texture);
    lc_sg_destroy(depthTexture);

    sg_image_desc imageDesc{};
    imageDesc.usage.color_attachment = true;
    imageDesc.usage.immutable = false;
    imageDesc.width = this->width;
    imageDesc.height = this->height;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    texture = sg_make_image(imageDesc);

    sg_image_desc depthDesc{};
    depthDesc.usage.depth_stencil_attachment = true;
    depthDesc.usage.immutable = false;
    depthDesc.width = this->width;
    depthDesc.height = this->height;
    depthDesc.pixel_format = sg_query_desc().environment.defaults.depth_format;
    depthTexture = sg_make_image(depthDesc);

    sg_view_desc colorViewDesc{};
    colorViewDesc.color_attachment.image = texture;
    colorView = sg_make_view(colorViewDesc);

    sg_view_desc depthViewDesc{};
    depthViewDesc.depth_stencil_attachment.image = depthTexture;
    depthView = sg_make_view(depthViewDesc);

    sg_view_desc texViewDesc{};
    texViewDesc.texture.image = texture;
    textureView = sg_make_view(texViewDesc);
}

ResizableFrameBuffer::~ResizableFrameBuffer() {
    lc_sg_destroy(textureView);
    lc_sg_destroy(colorView);
    lc_sg_destroy(depthView);
    lc_sg_destroy(texture);
    lc_sg_destroy(depthTexture);
}
