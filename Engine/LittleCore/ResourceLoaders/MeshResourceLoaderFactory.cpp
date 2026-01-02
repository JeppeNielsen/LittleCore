//
// Created by Jeppe Nielsen on 20/10/2025.
//

#include "MeshResourceLoaderFactory.hpp"
#include "MeshLoader.hpp"

using namespace LittleCore;

IResourceLoaderFactory<MeshResourceLoader>::Loader LittleCore::MeshResourceLoaderFactory::Create() {
    return CreateLoader();
}

bool MeshResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return MeshLoader::IsValidMesh(path);
}
