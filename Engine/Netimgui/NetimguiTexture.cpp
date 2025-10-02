//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include "NetimguiTexture.hpp"

using namespace LittleCore;

void NetimguiTexture::Resize(uint16_t width, uint16_t height) {
    if (this->width == width && this->height == height) {
        return;
    }
    this->width = width;
    this->height = height;

    pixels.resize(width * height * 4);

    if (bgfx::isValid(blitTexture)) {
        bgfx::destroy(blitTexture);
    }

    blitTexture = bgfx::createTexture2D(
            width, height, false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST);

}

NetimguiTexture::NetimguiTexture(bgfx::TextureHandle texture) : texture(texture) {

}

NetimguiTexture::~NetimguiTexture() {
    if (bgfx::isValid(blitTexture)) {
        bgfx::destroy(blitTexture);
        blitTexture = BGFX_INVALID_HANDLE;
    }
}
