//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include "SokolDirect.hpp"
#include <vector>

namespace LittleCore {
    struct NetimguiTexture {
        sg_image texture = {SG_INVALID_ID};
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> pixels;

        NetimguiTexture(sg_image texture);
        ~NetimguiTexture();

        void Resize(uint16_t width, uint16_t height);

    };
}
