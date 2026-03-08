//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include "SokolDirect.hpp"
#include <map>
#include <memory>
#include "NetimguiTexture.hpp"

namespace LittleCore {
    class NetimguiTextureSender {
    public:
        void SendTexture(sg_image texture, uint16_t width, uint16_t height);
        void FreeMemory();

    private:
        using Textures = std::map<uint32_t, std::unique_ptr<NetimguiTexture>>;
        Textures textures;
    };
}
