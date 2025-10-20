//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include "ResourceLoaderFactory.hpp"
#include "ShaderResourceLoader.hpp"

namespace LittleCore {
    struct ShaderResourceLoaderFactory : IResourceLoaderFactory<ShaderResourceLoader> {
        Loader Create() override;
        bool IsPathSupported(const std::string& path);
    };
}