//
// Created by Jeppe Nielsen on 16/12/2024.
//


#pragma once
#include <functional>
#include <imgui.h>
#include "Renderer.hpp"

class EditorRenderer {
public:

    using Callback = std::function<void(LittleCore::Renderer&)>;

    using RenderFunction = std::function<void(const std::string&, int, int, Callback)>;
    using GetTextureFunction = std::function<ImTextureID(const std::string&)>;

    EditorRenderer(RenderFunction renderFunction, GetTextureFunction getTextureFunction);

    void Render(const std::string& id, int width, int height, Callback callback);

    ImTextureID GetTexture(const std::string& id);

private:
    RenderFunction renderFunction;
    GetTextureFunction getTextureFunction;

};
