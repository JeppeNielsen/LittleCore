//
// Created by Jeppe Nielsen on 26/12/2024.
//

#include "ResourceLoader.hpp"

using namespace LittleCore;

ResourceLoader::ResourceLoader(ResourceLoader::LoadShaderFunction loadShaderFunction,
                               ResourceLoader::LoadTextureFunction loadTextureFunction) :
                               loadShaderFunction(loadShaderFunction),
                               loadTextureFunction(loadTextureFunction) {

}

LittleCore::ResourceHandle<LittleCore::ShaderResource> ResourceLoader::LoadShader(const std::string& id) {
    return loadShaderFunction(id);
}

LittleCore::ResourceHandle<LittleCore::TextureResource> ResourceLoader::LoadTexture(const std::string& id) {
    return loadTextureFunction(id);
}