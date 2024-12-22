//
// Created by Jeppe Nielsen on 16/12/2024.
//


#pragma once
#include <string>
#include <functional>
#include <imgui.h>
#include "Renderer.hpp"

class FrameBufferFactory;

class TextureRenderer {
public:

    TextureRenderer(LittleCore::Renderer& renderer);
    ~TextureRenderer();

    using Callback = std::function<void(LittleCore::Renderer&)>;

    void Render(const std::string& id, int width, int height, Callback callback);

    ImTextureID GetTexture(const std::string& id);

private:
    LittleCore::Renderer& renderer;
    FrameBufferFactory* frameBufferFactory;
};
