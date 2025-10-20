//
// Created by Jeppe Nielsen on 20/10/2025.
//

#include "ImageLoader.hpp"
#include "TextureResourceLoaderFactory.hpp"

using namespace LittleCore;

IResourceLoaderFactory<TextureResourceLoader>::Loader TextureResourceLoaderFactory::Create() {
    return CreateLoader();
}

bool TextureResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return ImageLoader::IsValidImageFile(path);
}