//
// Created by Jeppe Nielsen on 13/09/2025.
//


#pragma once
#include <string>
#include <bgfx/bgfx.h>
#include "Fiber.hpp"
#include "NetimguiTextureSender.hpp"

namespace LittleCore {

    class NetimguiClientController {

    public:

        ~NetimguiClientController();

        bool Start();
        void Connect(const std::string& name, const std::string& serverHost);
        void ShutDown();

        bool IsConnectionPending();
        bool IsConnected();
        void SendTexture(bgfx::TextureHandle texture, uint32_t width, uint32_t height);

        Fiber ConnectFlow(const std::string& clientName, const std::string& serverHost, bool& didConnect);

    private:
        NetimguiTextureSender textureSender;
    };


}