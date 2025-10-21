//
// Created by Jeppe Nielsen on 20/10/2025.
//
#include "ShaderResourceLoaderFactory.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;


IResourceLoaderFactory<ShaderResourceLoader>::Loader ShaderResourceLoaderFactory::Create() {
    return CreateLoader();
}

bool ShaderResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return FileHelper::HasExtension(path, "shader");
}
