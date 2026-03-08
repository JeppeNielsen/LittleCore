//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "ShaderResourceLoader.hpp"

using namespace LittleCore;

void ShaderResourceLoader::Load(ShaderResource& resource) {
    // Direct sokol pipeline integration is pending (sokol-shdc pipeline rewrite).
    resource.handle = {SG_INVALID_ID};
}

void ShaderResourceLoader::Unload(ShaderResource& resource) {
    if (resource.handle.id != SG_INVALID_ID) {
        sg_destroy_shader(resource.handle);
        resource.handle = {SG_INVALID_ID};
    }
}

bool ShaderResourceLoader::IsLoaded() {
    return true;
}

void ShaderResourceLoader::Reload(ShaderResource& resource) {
    Unload(resource);
    Load(resource);
}

