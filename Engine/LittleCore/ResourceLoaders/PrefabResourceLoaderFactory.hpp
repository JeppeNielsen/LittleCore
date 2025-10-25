//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceLoaderFactory.hpp"
#include "PrefabResourceLoader.hpp"
#include "RegistrySerializer.hpp"

namespace LittleCore {
    struct PrefabResourceLoaderFactory  : IResourceLoaderFactory<PrefabResourceLoader> {
    public:

        PrefabResourceLoaderFactory(RegistrySerializerBase& registrySerializer);

        Loader Create() override;
        bool IsPathSupported(const std::string& path);

    private:
        RegistrySerializerBase& registrySerializer;
    };
}
