//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include "SokolDirect.hpp"
#include <functional>

namespace LittleCore {
    class ResizableFrameBuffer {
    public:
        ~ResizableFrameBuffer();
        void Render(uint16_t width, uint16_t height, const std::function<void()>& renderFunction);
        void EnsureResources(uint16_t width, uint16_t height);
        uint16_t width = 0;
        uint16_t height = 0;
        sg_image texture = {SG_INVALID_ID};
        sg_image depthTexture = {SG_INVALID_ID};
        sg_attachments frameBuffer = {SG_INVALID_ID};
    };
}
