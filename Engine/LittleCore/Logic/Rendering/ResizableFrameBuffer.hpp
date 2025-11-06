//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include <functional>

namespace LittleCore {
    class ResizableFrameBuffer {
    public:
        ~ResizableFrameBuffer();
        void Render(uint16_t width, uint16_t height, const std::function<void()>& renderFunction);
        void EnsureResources(uint16_t width, uint16_t height);
        uint16_t width = 0;
        uint16_t height = 0;
        bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
        bgfx::FrameBufferHandle frameBuffer = BGFX_INVALID_HANDLE;
    };
}
