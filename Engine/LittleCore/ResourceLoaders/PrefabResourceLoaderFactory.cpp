//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabResourceLoaderFactory.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

IResourceLoaderFactory<PrefabResourceLoader>::Loader PrefabResourceLoaderFactory::Create() {
    return CreateLoader(registrySerializer);
}

bool PrefabResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return FileHelper::HasExtension(path, "prefab");
}

PrefabResourceLoaderFactory::PrefabResourceLoaderFactory(RegistrySerializerBase& registrySerializer) : registrySerializer(registrySerializer) {

}
