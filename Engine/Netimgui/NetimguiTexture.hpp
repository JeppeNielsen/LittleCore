//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include <vector>

namespace LittleCore {
    struct NetimguiTexture {
        bgfx::TextureHandle texture;
        bgfx::TextureHandle blitTexture = BGFX_INVALID_HANDLE;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> pixels;

        NetimguiTexture(bgfx::TextureHandle texture);
        ~NetimguiTexture();

        void Resize(uint16_t width, uint16_t height);

    };
}
