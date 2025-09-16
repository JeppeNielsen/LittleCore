//
// Created by Jeppe Nielsen on 13/09/2025.
//

#define NETIMGUI_IMPLEMENTATION
#include <NetImgui_Api.h>

#include "NetimguiClient.hpp"
using namespace LittleCore;

bool NetimguiClient::Start() {
    return NetImgui::Startup();
}

void NetimguiClient::ShutDown() {
    NetImgui::Shutdown();
}

void NetimguiClient::Connect(const std::string& name, const std::string& serverHost) {
    NetImgui::ConnectToApp(name.c_str(), serverHost.c_str());
}

bool NetimguiClient::IsConnectionPending() {
    return NetImgui::IsConnectionPending();
}

bool NetimguiClient::IsConnected() {
    return NetImgui::IsConnected();
}

NetimguiClient::~NetimguiClient() {
    NetImgui::Shutdown();
}

void NetimguiClient::SendTexture(bgfx::TextureHandle texture, bgfx::TextureHandle textureBlit, uint32_t width, uint32_t height, std::vector<uint8_t>& pixels) {

    const bgfx::ViewId kCopyView = 250;
    bgfx::blit(kCopyView, textureBlit, 0, 0, texture);
    uint32_t readyFrame = bgfx::readTexture(textureBlit, pixels.data());

    while (bgfx::frame(false)< readyFrame) {

    }

    NetImgui::SendDataTexture((ImTextureID) (uintptr_t) texture.idx, pixels.data(), width, height,
                              NetImgui::eTexFormat::kTexFmtRGBA8);
}

