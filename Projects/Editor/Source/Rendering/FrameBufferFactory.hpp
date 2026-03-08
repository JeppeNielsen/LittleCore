//
// Created by Jeppe Nielsen on 16/12/2024.
//

#pragma once
#include "SokolDirect.hpp"
#include <string>
#include <unordered_map>

class FrameBufferFactory {
public:

    struct FrameBuffer {
        std::string id;
        int width = 0;
        int height = 0;
        sg_image renderTexture = {SG_INVALID_ID};
        sg_attachments framebuffer = {SG_INVALID_ID};
    };

    FrameBuffer& CreateBuffer(const std::string& id, int width, int height);

    bool TryGetFrameBuffer(const std::string& id, FrameBuffer** frameBuffer);

private:
    std::unordered_map<std::string, FrameBuffer> frameBuffers;
};
