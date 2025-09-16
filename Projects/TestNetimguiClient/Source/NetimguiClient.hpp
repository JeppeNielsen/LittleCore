//
// Created by Jeppe Nielsen on 13/09/2025.
//


#pragma once
#include <string>
#include <bgfx/bgfx.h>

namespace LittleCore {

    class NetimguiClient {

    public:

        ~NetimguiClient();

        bool Start();
        void Connect(const std::string& name, const std::string& serverHost);
        void ShutDown();

        bool IsConnectionPending();
        bool IsConnected();
        void SendTexture(bgfx::TextureHandle texture, bgfx::TextureHandle textureBlit, uint32_t width, uint32_t height, std::vector<uint8_t>& pixels);
    };


}