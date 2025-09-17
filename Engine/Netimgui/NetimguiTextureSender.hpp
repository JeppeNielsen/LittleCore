//
// Created by Jeppe Nielsen on 17/09/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include <map>
#include <memory>
#include "NetimguiTexture.hpp"

namespace LittleCore {
    class NetimguiTextureSender {
    public:
        void SendTexture(bgfx::TextureHandle texture, uint16_t width, uint16_t height);
        void FreeMemory();

    private:
        using Textures = std::map<uint16_t, std::unique_ptr<NetimguiTexture>>;
        Textures textures;
    };
}
