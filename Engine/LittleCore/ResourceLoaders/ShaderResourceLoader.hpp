//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include <vector>
#include "ResourceLoader.hpp"
#include "ShaderResource.hpp"

namespace LittleCore {
    class ShaderResourceLoader : public IResourceLoader<ShaderResource> {

        void Load(ShaderResource& resource) override;
        void Unload(ShaderResource& resource) override;
        bool IsLoaded() override;
        void Reload(ShaderResource& resource) override;

        std::vector<uint8_t> vertexShader;
        std::vector<uint8_t> fragmentShader;

    };
}

