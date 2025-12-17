//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include "ResourceLoaderFactory.hpp"
#include "FontResourceLoader.hpp"

namespace LittleCore {
    struct FontResourceLoaderFactory : IResourceLoaderFactory<FontResourceLoader> {
        Loader Create() override;
        ~FontResourceLoaderFactory();
        bool IsPathSupported(const std::string& path);
    };
}