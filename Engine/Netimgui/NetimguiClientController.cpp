//
// Created by Jeppe Nielsen on 13/09/2025.
//

#define NETIMGUI_IMPLEMENTATION
#include <NetImgui_Api.h>

#include "NetimguiClientController.hpp"
using namespace LittleCore;

bool NetimguiClientController::Start() {
    return NetImgui::Startup();
}

void NetimguiClientController::ShutDown() {
    NetImgui::Shutdown();
}

void NetimguiClientController::Connect(const std::string& name, const std::string& serverHost) {
    NetImgui::ConnectToApp(name.c_str(), serverHost.c_str());
}

bool NetimguiClientController::IsConnectionPending() {
    return NetImgui::IsConnectionPending();
}

bool NetimguiClientController::IsConnected() {
    return NetImgui::IsConnected();
}

NetimguiClientController::~NetimguiClientController() {
    NetImgui::Shutdown();
}

void NetimguiClientController::SendTexture(bgfx::TextureHandle texture, bgfx::TextureHandle textureBlit, uint32_t width, uint32_t height, std::vector<uint8_t>& pixels) {

    const bgfx::ViewId kCopyView = 250;
    bgfx::blit(kCopyView, textureBlit, 0, 0, texture);
    uint32_t readyFrame = bgfx::readTexture(textureBlit, pixels.data());

    while (bgfx::frame(false)< readyFrame) {

    }

    NetImgui::SendDataTexture((ImTextureID) (uintptr_t) texture.idx, pixels.data(), width, height,
                              NetImgui::eTexFormat::kTexFmtRGBA8);
}

