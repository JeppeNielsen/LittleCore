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

