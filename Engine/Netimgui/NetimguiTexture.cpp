//
// Created by Jeppe Nielsen on 17/09/2025.
//

#include "NetimguiTexture.hpp"

using namespace LittleCore;

void NetimguiTexture::Resize(uint16_t width, uint16_t height) {
    if (this->width == width && this->height == height) {
        return;
    }
    this->width = width;
    this->height = height;

    pixels.resize(width * height * 4);
}

NetimguiTexture::NetimguiTexture(sg_image texture) : texture(texture) {

}

NetimguiTexture::~NetimguiTexture() {
}
