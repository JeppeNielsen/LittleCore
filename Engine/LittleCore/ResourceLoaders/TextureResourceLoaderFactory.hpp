//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include "ResourceLoaderFactory.hpp"
#include "TextureResourceLoader.hpp"

namespace LittleCore {
    struct TextureResourceLoaderFactory : IResourceLoaderFactory<TextureResourceLoader> {
        Loader Create() override;
        bool IsPathSupported(const std::string& path);
    };
}