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

void NetimguiClientController::SendTexture(bgfx::TextureHandle texture, uint32_t width, uint32_t height) {
    textureSender.SendTexture(texture, width, height);
}

Fiber NetimguiClientController::ConnectFlow(const std::string& clientName, const std::string& serverHost, bool& didConnect) {

    while (IsConnectionPending()) {
        std::this_thread::sleep_for(std::chrono::milliseconds (16));
        co_yield 0;
    }

    didConnect = IsConnected();
}

