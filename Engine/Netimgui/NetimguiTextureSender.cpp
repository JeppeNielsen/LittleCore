//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include <NetImgui_Api.h>
#include "NetimguiTextureSender.hpp"

using namespace LittleCore;



void NetimguiTextureSender::SendTexture(bgfx::TextureHandle texture, uint16_t width, uint16_t height) {

    NetimguiTexture* netimguiTexture;
    auto it = textures.find(texture.idx);
    if (it == textures.end()) {
        it = textures.emplace(texture.idx, std::make_unique<NetimguiTexture>(texture)).first;
    }
    netimguiTexture = it->second.get();
    netimguiTexture->Resize(width, height);

    const bgfx::ViewId kCopyView = 250;
    bgfx::blit(kCopyView, netimguiTexture->blitTexture, 0, 0, texture);
    uint32_t readyFrame = bgfx::readTexture(netimguiTexture->blitTexture, netimguiTexture->pixels.data());

    while (bgfx::frame(false)< readyFrame) {

    }

    NetImgui::SendDataTexture((ImTextureID) (uintptr_t) texture.idx, netimguiTexture->pixels.data(), width, height,
            NetImgui::eTexFormat::kTexFmtRGBA8);
}

void NetimguiTextureSender::FreeMemory() {
    textures.clear();
}

