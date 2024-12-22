//
// Created by Jeppe Nielsen on 16/12/2024.
//

#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>

class FrameBufferFactory {
public:

    struct FrameBuffer {
        std::string id;
        bgfx::TextureHandle renderTexture;
        bgfx::FrameBufferHandle framebuffer;
    };

    FrameBuffer& CreateBuffer(const std::string& id, int width, int height);

    bool TryGetFrameBuffer(const std::string& id, FrameBuffer** frameBuffer);

private:
    std::unordered_map<std::string, FrameBuffer> frameBuffers;
};