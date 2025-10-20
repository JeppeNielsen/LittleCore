//
// Created by Jeppe Nielsen on 20/10/2025.
//

#include "MeshResourceLoaderFactory.hpp"

using namespace LittleCore;

IResourceLoaderFactory<MeshResourceLoader>::Loader LittleCore::MeshResourceLoaderFactory::Create() {
    return CreateLoader();
}

bool MeshResourceLoaderFactory::IsPathSupported(const std::string& path) {
    MeshLoader meshLoader;
    return meshLoader.IsValidMesh(path);
}
