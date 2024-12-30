//
// Created by Jeppe Nielsen on 11/12/2024.
//

#pragma once
#include <functional>
#include "ResourceHandle.hpp"
#include "ShaderResource.hpp"
#include "TextureResource.hpp"
#include <string>

class ResourceLoader {
public:

    using LoadShaderFunction = std::function<LittleCore::ResourceHandle<LittleCore::ShaderResource>(const std::string&)>;
    using LoadTextureFunction = std::function<LittleCore::ResourceHandle<LittleCore::TextureResource>(const std::string&)>;

    ResourceLoader(LoadShaderFunction loadShaderFunction, LoadTextureFunction loadTextureFunction);

    LittleCore::ResourceHandle<LittleCore::ShaderResource> LoadShader(const std::string& id);
    LittleCore::ResourceHandle<LittleCore::TextureResource> LoadTexture(const std::string& id);

private:

    LoadShaderFunction loadShaderFunction;
    LoadTextureFunction loadTextureFunction;
};