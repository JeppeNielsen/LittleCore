//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include "ResizableFrameBuffer.hpp"

using namespace LittleCore;

void ResizableFrameBuffer::Render(uint16_t width, uint16_t height, const std::function<void()>& renderFunction) {
    EnsureResources(width, height);
    bgfx::setViewFrameBuffer(0, frameBuffer);
    renderFunction();
    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
}

void ResizableFrameBuffer::EnsureResources(uint16_t width, uint16_t height) {
    if (this->width == width && this->height == height) {
        return;
    }

    this->width = width;
    this->height = height;

    if (bgfx::isValid(texture)) {
        bgfx::destroy(texture);
    }

    if (bgfx::isValid(frameBuffer)) {
        bgfx::destroy(frameBuffer);
    }

    const bgfx::TextureFormat::Enum kFormat = bgfx::TextureFormat::RGBA8;
    const uint64_t kTexFlags = BGFX_TEXTURE_RT; // RT texture; sampler filtering is set when binding.

    texture = bgfx::createTexture2D(
            this->width,
            this->height,
            false,
            1,
            kFormat,
            kTexFlags
    );

    frameBuffer = bgfx::createFrameBuffer(1, &texture, false);
}

ResizableFrameBuffer::~ResizableFrameBuffer() {
    if (bgfx::isValid(texture)) {
        bgfx::destroy(texture);
        texture = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(frameBuffer)) {
        bgfx::destroy(frameBuffer);
        frameBuffer = BGFX_INVALID_HANDLE;
    }
}


