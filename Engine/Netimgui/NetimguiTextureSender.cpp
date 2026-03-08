//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include <NetImgui_Api.h>
#include "NetimguiTextureSender.hpp"

using namespace LittleCore;



void NetimguiTextureSender::SendTexture(sg_image texture, uint16_t width, uint16_t height) {

    NetimguiTexture* netimguiTexture;
    auto it = textures.find(texture.id);
    if (it == textures.end()) {
        it = textures.emplace(texture.id, std::make_unique<NetimguiTexture>(texture)).first;
    }
    netimguiTexture = it->second.get();
    netimguiTexture->Resize(width, height);

    NetImgui::SendDataTexture((ImTextureID) (uintptr_t) texture.id, netimguiTexture->pixels.data(), width, height,
            NetImgui::eTexFormat::kTexFmtRGBA8);
}

void NetimguiTextureSender::FreeMemory() {
    textures.clear();
}
